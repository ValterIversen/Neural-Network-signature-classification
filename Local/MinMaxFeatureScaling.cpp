
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "CppProject/CppProject/MinMaxFeatureScaling.h"

using namespace std;

//Get the max value from the vector
double MinMaxFeatureScaling::max_element(vector<double> values)
{
	double largest = values[0];
	for (int i = 1; i < values.size(); ++i) {
		if (values[i] > largest)
			largest = values[i];
	}
	return largest;
};

//Get the minimun value from the vector
double MinMaxFeatureScaling::min_element(vector<double> values)
{
	double smaller = values[0];
	for (int i = 1; i < values.size(); ++i) {
		if (values[i] < smaller)
			smaller = values[i];
	}
	return smaller;
};

inline string MinMaxFeatureScaling::as_string(double value) {
	char buf[32];
	return std::string(buf, std::snprintf(buf, sizeof buf, "%.16g", value));
};

//Take the max/min elements and aplly the minmaxfeaturescaling
vector<double> MinMaxFeatureScaling::getMinMaxFeatureScaling(vector<double>& values){
	double maxValue = MinMaxFeatureScaling::max_element(values);
	double minValue = MinMaxFeatureScaling::min_element(values);

	minMax.push_back(maxValue);
	minMax.push_back(minValue);
	
	for(int i = 0; i < values.size(); ++i){
		values[i] = (values[i] - minValue) / (maxValue - minValue);
	}
	return values;
}


//Save the file with the min and max values for the android 
void MinMaxFeatureScaling::saveFileMinMaxFeatureScaling() {

	string FileData = "";

	for (int i = 0; i < minMax.size(); i++) {
		FileData += as_string(minMax[i]) + " ";
	}

	ofstream m_minMaxFile;
	string fileName = "DataSet\\minMaxFile.txt";

	m_minMaxFile.open(fileName, fstream::in | fstream::out | fstream::trunc);

	m_minMaxFile << FileData;

	m_minMaxFile.close();
}

vector<double> MinMaxFeatureScaling::readFileMinMaxFeatureScaling() {
	ifstream m_minMaxFile;
	string fileName = "DataSet\\minMaxFile.txt";
	m_trainingDataFile.open(fileName.c_str());

	vector<double> minMaxFile;
	while (!m_trainingDataFile.eof()) {
		string line;
		getline(m_trainingDataFile, line);

		stringstream ss(line);

		double oneValue;
		while (ss >> oneValue) {
			minMaxFile.push_back(oneValue);
		}
	}
	return minMaxFile;
}

void MinMaxFeatureScaling::getTrainingMinMax() {

	m_trainingDataFile.open(trainningFileName.c_str());

	//create a bunch of vectors to store the values of the training data set
	vector<string> fileData;
	vector<string> newFileData;
	vector<double> totalTime;
	vector<double> sumTimePerTrajectory;
	vector<double> fingersUp;
	vector<double> sumTrajectoryInX;
	vector<double> sumTrajectoryInY;
	vector<double> sumTrajectoryOutX;
	vector<double> sumTrajectoryOutY;

	while (!m_trainingDataFile.eof()) {
		string line;
		getline(m_trainingDataFile, line);

		stringstream ss(line);
		string newLine;

		ss >> newLine;

		int valuesCount = 1;
		double oneValue;
		while (ss >> oneValue) {
			if (valuesCount < 7) {
				newLine += " " + as_string(oneValue);
			}
			else {
				switch (valuesCount)
				{
				case 7:
					totalTime.push_back(oneValue);
					break;
				case 8:
					sumTimePerTrajectory.push_back(oneValue);
					break;
				case 9:
					fingersUp.push_back(oneValue);
					break;
				case 10:
					sumTrajectoryInX.push_back(oneValue);
					break;
				case 11:
					sumTrajectoryInY.push_back(oneValue);
					break;
				case 12:
					sumTrajectoryOutX.push_back(oneValue);
					break;
				case 13:
					sumTrajectoryOutY.push_back(oneValue);
					break;
				}
			}

			++valuesCount;
		}
		newFileData.push_back(newLine);
	}
	m_trainingDataFile.close();

	totalTime = getMinMaxFeatureScaling(totalTime);
	sumTimePerTrajectory = getMinMaxFeatureScaling(sumTimePerTrajectory);
	fingersUp = getMinMaxFeatureScaling(fingersUp);
	sumTrajectoryInX = getMinMaxFeatureScaling(sumTrajectoryInX);
	sumTrajectoryInY = getMinMaxFeatureScaling(sumTrajectoryInY);
	sumTrajectoryOutX = getMinMaxFeatureScaling(sumTrajectoryOutX);
	sumTrajectoryOutY = getMinMaxFeatureScaling(sumTrajectoryOutY);


	for (int i = 0; i < totalTime.size() * 2; i += 2) {
		string newLine = "";
		if (i > 0)
			newLine += " " + as_string(totalTime[i / 2]) + " " + as_string(sumTimePerTrajectory[i / 2]) + " " + as_string(fingersUp[i / 2]) + " " +
			as_string(sumTrajectoryInX[i / 2]) + " " + as_string(sumTrajectoryInY[i / 2]) + " " + as_string(sumTrajectoryOutX[i / 2]) + " " + as_string(sumTrajectoryOutY[i / 2]);
		else
			newLine += " " + as_string(totalTime[0]) + " " + as_string(sumTimePerTrajectory[0]) + " " + as_string(fingersUp[0]) + " " +
			as_string(sumTrajectoryInX[0]) + " " + as_string(sumTrajectoryInY[0]) + " " + as_string(sumTrajectoryOutX[0]) + " " + as_string(sumTrajectoryOutY[0]);
		newFileData[i] += newLine;
	}


	string FileData = "";
	for (int i = 0; i < newFileData.size(); i++)
		FileData += newFileData[i] + "\n";


	m_trainingDataFileNormalized.open(finalTrainningFileName, fstream::in | fstream::out | fstream::trunc);

	m_trainingDataFileNormalized << FileData;

	m_trainingDataFileNormalized.close();
}

void MinMaxFeatureScaling::normalizeTestData() {
	m_testDataFile.open(testFileName.c_str());

	//create a bunch of vectors to store the values of the test data set
	vector<string> fileData;
	vector<string> newFileData;
	vector<double> totalTime;
	vector<double> sumTimePerTrajectory;
	vector<double> fingersUp;
	vector<double> sumTrajectoryInX;
	vector<double> sumTrajectoryInY;
	vector<double> sumTrajectoryOutX;
	vector<double> sumTrajectoryOutY;

	while (!m_testDataFile.eof()) {
		string line;
		getline(m_testDataFile, line);

		stringstream ss(line);
		string newLine;

		ss >> newLine;

		int valuesCount = 1;
		double oneValue;
		while (ss >> oneValue) {
			if (valuesCount < 7) {
				newLine += " " + as_string(oneValue);
			}
			else {
				switch (valuesCount)
				{
				case 7:
					totalTime.push_back(oneValue);
					break;
				case 8:
					sumTimePerTrajectory.push_back(oneValue);
					break;
				case 9:
					fingersUp.push_back(oneValue);
					break;
				case 10:
					sumTrajectoryInX.push_back(oneValue);
					break;
				case 11:
					sumTrajectoryInY.push_back(oneValue);
					break;
				case 12:
					sumTrajectoryOutX.push_back(oneValue);
					break;
				case 13:
					sumTrajectoryOutY.push_back(oneValue);
					break;
				}
			}

			++valuesCount;
		}
		newFileData.push_back(newLine);
	}
	m_testDataFile.close();


	//Did not use the getMinMaxFeatureScaling like the getTrainingMinMax() because we alredy have the minmax values from the training
	double max = minMax[0];
	double min = minMax[1];
	for (int i = 0; i < totalTime.size(); ++i) {
		totalTime[i] = (totalTime[i] - min) / (max - min);
	}

	max = minMax[2];
	min = minMax[3];
	for (int i = 0; i < sumTimePerTrajectory.size(); ++i) {
		sumTimePerTrajectory[i] = (sumTimePerTrajectory[i] - min) / (max - min);
	}

	max = minMax[4];
	min = minMax[5];
	for (int i = 0; i < fingersUp.size(); ++i) {
		fingersUp[i] = (fingersUp[i] - min) / (max - min);
	}

	max = minMax[6];
	min = minMax[7];
	for (int i = 0; i < sumTrajectoryInX.size(); ++i) {
		sumTrajectoryInX[i] = (sumTrajectoryInX[i] - min) / (max - min);
	}

	max = minMax[8];
	min = minMax[9];
	for (int i = 0; i < sumTrajectoryInY.size(); ++i) {
		sumTrajectoryInY[i] = (sumTrajectoryInY[i] - min) / (max - min);
	}

	max = minMax[10];
	min = minMax[11];
	for (int i = 0; i < sumTrajectoryOutX.size(); ++i) {
		sumTrajectoryOutX[i] = (sumTrajectoryOutX[i] - min) / (max - min);
	}

	max = minMax[12];
	min = minMax[13];
	for (int i = 0; i < sumTrajectoryOutY.size(); ++i) {
		sumTrajectoryOutY[i] = (sumTrajectoryOutY[i] - min) / (max - min);
	}


	for (int i = 0; i < totalTime.size() * 2; i += 2) {
		string newLine = "";
		if (i > 0)
			newLine += " " + as_string(totalTime[i / 2]) + " " + as_string(sumTimePerTrajectory[i / 2]) + " " + as_string(fingersUp[i / 2]) + " " +
			as_string(sumTrajectoryInX[i / 2]) + " " + as_string(sumTrajectoryInY[i / 2]) + " " + as_string(sumTrajectoryOutX[i / 2]) + " " + as_string(sumTrajectoryOutY[i / 2]);
		else
			newLine += " " + as_string(totalTime[0]) + " " + as_string(sumTimePerTrajectory[0]) + " " + as_string(fingersUp[0]) + " " +
			as_string(sumTrajectoryInX[0]) + " " + as_string(sumTrajectoryInY[0]) + " " + as_string(sumTrajectoryOutX[0]) + " " + as_string(sumTrajectoryOutY[0]);
		newFileData[i] += newLine;
	}


	string FileData = "";
	for (int i = 0; i < newFileData.size(); i++)
		FileData += newFileData[i] + "\n";


	m_testDataFileNormalized.open(finalTestFileName, fstream::in | fstream::out | fstream::trunc);

	m_testDataFileNormalized << FileData;

	m_testDataFileNormalized.close();
}

MinMaxFeatureScaling::MinMaxFeatureScaling(const string trainningFileNameP, const string finalTrainningFileNameP, const string testFileNameP, const string finalTestFileNameP){
	trainningFileName = trainningFileNameP;
	finalTrainningFileName = finalTrainningFileNameP;
	testFileName = testFileNameP;
	finalTestFileName = finalTestFileNameP;
	getTrainingMinMax();

	saveFileMinMaxFeatureScaling();

	normalizeTestData();


	cout << "DataSet normalizado.";
}

