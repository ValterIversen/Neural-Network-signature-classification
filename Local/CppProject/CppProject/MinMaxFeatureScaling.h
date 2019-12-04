

#pragma once
#ifndef MINMAXFEATURESCALING_H
#define MINMAXFEATURESCALING_H

//This class is responsible to normalize the values,
// and save the values in two files
class MinMaxFeatureScaling;

class MinMaxFeatureScaling {
public:
	MinMaxFeatureScaling(const std::string trainningFileName, const std::string finalTrainningFileName, const std::string testFileName, const std::string finalTestFileName);
private:
	std::string trainningFileName;
	std::string finalTrainningFileName;
	std::string testFileName;
	std::string finalTestFileName;
	double min_element(std::vector<double> values);
	double max_element(std::vector<double> values);
	std::vector<double> minMax;
	inline std::string as_string(double value);
	std::ifstream m_trainingDataFile;
	std::ofstream m_trainingDataFileNormalized;
	std::ifstream m_testDataFile;
	std::ofstream m_testDataFileNormalized;
	std::vector<double> getMinMaxFeatureScaling(std::vector<double>& values);
	std::vector<double> getMinMaxFeatureScalingFromFile(std::vector<double>& values);
	std::vector<double> readFileMinMaxFeatureScaling();
	void saveFileMinMaxFeatureScaling();
	void getTrainingMinMax();
	void normalizeTestData();
};

#endif // !MINMAXFEATURESCALING_H