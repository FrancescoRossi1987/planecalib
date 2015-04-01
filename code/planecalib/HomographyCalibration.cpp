
#include "HomographyCalibration.h"

#include <memory>
#include <ceres/ceres.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "log.h"
#include "cvutils.h"
#include "CeresParametrization.h"

namespace planecalib {

void HomographyCalibration::calibrate(const std::vector<Eigen::Matrix3fr> &H, const Eigen::Vector2i &imageSize)
{
	int hcount = H.size();

	//Adjust for principal point
	Eigen::Matrix3fr T, Ti;
	float halfWidth = (float)imageSize.x()/2;
	float halfHeight = (float)imageSize.y() / 2;
	T << 1, 0, -halfWidth, 0, 1, -halfHeight, 0, 0, 1;
	Ti << 1, 0, +halfWidth / 2, 0, 1, +halfHeight / 2, 0, 0, 1;

	std::vector<Eigen::Matrix3d> Ht(hcount);
	for (int i = 0; i < hcount; i++)
	{
		Ht[i] = (T * H[i] * Ti).cast<double>();
		//Ht[i] /= Ht[i].determinant();
	}

	//Assume normal is [0,0,1] and find focal length
	Eigen::VectorXd A(2 * hcount), b(2 * hcount);
	for (int i = 0; i < hcount; i++)
	{
		auto &Hti = Ht[i];

		//Note: Hti must be column-major!
		A[2 * i] = -Hti(2)*Hti(5);
		b[2 * i] = Hti(0)*Hti(3) + Hti(1)*Hti(4);
		A[2 * i + 1] = Hti(5)*Hti(5) - Hti(2)*Hti(2);
		b[2 * i + 1] = -(Hti(3)*Hti(3) - Hti(0)*Hti(0) + Hti(4)*Hti(4) - Hti(1)*Hti(1));
	}

	double alpha = sqrt(A.dot(b) / (A.dot(A))); //alpha = (pinv(A)*b)^0.5;
	mInitialAlpha = alpha;

	//Non-linear minimization
	ceres::Problem problem;
	ceres::LossFunction *lossFunc = new ceres::CauchyLoss(3.0);

	Eigen::Vector2d pp(0,0); //Assume principal point is in the middle
	mNormal << 0, 0, 1; //Assume reference camera is perependicular to plane.

	problem.AddParameterBlock(&alpha, 1);
	problem.AddParameterBlock(pp.data(), 2);
	//problem.SetParameterBlockConstant(pp.data());
	problem.AddParameterBlock(mNormal.data(), 3, new Fixed3DNormParametrization(1));
	//problem.SetParameterBlockConstant(mNormal.data());
	for (int i = 0; i<hcount; i++)
	{
		problem.AddResidualBlock(
			new ceres::AutoDiffCostFunction<HomographyCalibrationError, HomographyCalibrationError::kResidualCount, 1, 2, 3>(
			new HomographyCalibrationError(Ht[i])),
			lossFunc, &alpha,pp.data(),mNormal.data());
	}

	ceres::Solver::Options options;
	options.gradient_tolerance = 1e-25;
	options.parameter_tolerance = 1e-25;
	options.function_tolerance = 1e-25;
	options.linear_solver_type = ceres::DENSE_QR;
	options.minimizer_progress_to_stdout = false;

	ceres::Solver::Summary summary;

	ceres::Solve(options, &problem, &summary);
	MYAPP_LOG << summary.FullReport();

	MYAPP_LOG << "Initial alpha=" << alpha << ", final=" << alpha << "\n";

	//Build final K
	mK << (float)alpha, 0, (float)pp.x()+halfWidth, 0, (float)alpha, (float)pp.y()+halfHeight, 0, 0, 1;
}

} 