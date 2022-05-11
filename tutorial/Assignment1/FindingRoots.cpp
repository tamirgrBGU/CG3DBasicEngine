Eigen::Vector3cf Assignment1::FindCubicRoots() 
{
	Eigen::Vector2cf reduceCoeffs = Eigen::Vector2cf::Zero();
	Eigen::Vector3cf roots;
	std::complex<float> bOver3a = (coeffs[1]/coeffs[0])/3.0f;
	reduceCoeffs[0] = coeffs[2]/coeffs[0] - 3.0f*bOver3a*bOver3a;
	reduceCoeffs[1] = coeffs[2]/coeffs[0]*bOver3a - coeffs[3]/coeffs[0] - 2.0f*bOver3a*bOver3a*bOver3a;
	// std::cout<<"reduced\n"<<reduceCoeffs<<std::endl;
	if(reduceCoeffs.norm()>0.000001)
	{
		roots =	FindRootsOfReduceEquation(reduceCoeffs);
		roots[0] -= bOver3a;
		roots[1] -= bOver3a;
		roots[2] -= bOver3a;
	}
	else
	{
		roots[0] = -1.0f*bOver3a;
		roots[1] = std::complex<float>(std::cosf(3.14159f/3.0f),std::sinf(3.14159f/3.0f))*bOver3a;
		roots[2] = std::complex<float>(std::cosf(2.0f*3.14159f/3.0f),std::sinf(2*3.14159f/3.0f))*bOver3a;
	}

	return roots;
}

std::complex<float> Assignment1::NewtonCubicRoot(std::complex<float> num)
{
	std::complex<float> root = num;
	const int iter = 9;
	bool isSmall = false;
	if (std::abs(num) < 1e-3)
	{
		if (std::abs(num) == 0)
			return num;
		isSmall = true;
		num = num * 1e6f;
		root = num;
	}
	else
		if (std::abs(num) < 0.9f)
			root = 1;
	for (int k = 0; k < iter; k++)
	{
		root = (2.0f * root * root * root + num) / root / root / 3.0f;
	}
	if (isSmall)
		root = root / 100.0f;
	return root;
}

Eigen::Vector3cf Assignment1::FindRootsOfReduceEquation(Eigen::Vector2cf reduceCoeffs)
{		
	Eigen::Vector3cf roots = Eigen::Vector3cf::Zero();
	std::complex<float> sqroot = std::sqrt(reduceCoeffs[0]*reduceCoeffs[0]*reduceCoeffs[0]/27.0f + reduceCoeffs[1]*reduceCoeffs[1]/4.0f);
	std::complex<float> p = NewtonCubicRoot(reduceCoeffs[1]/2.0f + sqroot);
	std::complex<float> n = NewtonCubicRoot(reduceCoeffs[1]/2.0f - sqroot);
	roots[0] = p + n;
	roots[1] = p *std::complex<float>(std::cosf(2.0f*3.14159f/3.0f),std::sinf(2*3.14159f/3.0f)) - n * std::complex<float>(std::cosf(1.0f*3.14159f/3.0f),std::sinf(1*3.14159f/3.0f));
	roots[2] = -p * std::complex<float>(std::cosf(1.0f*3.14159f/3.0f),std::sinf(1*3.14159f/3.0f)) + n * std::complex<float>(std::cosf(2.0f*3.14159f/3.0f),std::sinf(2*3.14159f/3.0f));
	return roots;
}	