//The code was written in a hurry, i need to improve it in the future
//The project was the final work at graduation

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include "CppProject/CppProject/MinMaxFeatureScaling.h"

using namespace std;

//
class TrainingData
{
public:
    TrainingData(const string filename);
    bool isEof(void) { return m_trainingDataFile.eof(); }
    void getTopology(vector<unsigned> &topology);
	void popTopology();
	vector<vector<double>> readFileMinMaxFeatureScaling();

    // Returns the number of input values read from the file
    unsigned getNextInputs(vector<double> &inputVals);
    unsigned getTargetOutputs(vector<double> &targetOutputVals);

private:
    ifstream m_trainingDataFile;
};

void TrainingData::popTopology() {
	string line;

	getline(m_trainingDataFile, line);
}

void TrainingData::getTopology(vector<unsigned> &topology)
{
    string line;
    string label;

    getline(m_trainingDataFile, line);
    stringstream ss(line);
    ss >> label;
    if (this->isEof() || label.compare("topology:") != 0) {
        abort();
    }

    while (!ss.eof()) {
        unsigned n;
        ss >> n;
        topology.push_back(n);
    }

    return;
}

TrainingData::TrainingData(const string filename)
{
    m_trainingDataFile.open(filename.c_str());
}

// Returns the number of input values read from the file
unsigned TrainingData::getNextInputs(vector<double> &inputVals)
{
    inputVals.clear();

    string line;
    getline(m_trainingDataFile, line);
    stringstream ss(line);

    string label;
    ss>> label;
    if (label.compare("in:") == 0) {
        double oneValue;
        while (ss >> oneValue) {
            inputVals.push_back(oneValue);
        }
    }

    return inputVals.size();
}

// Returns the number of input values read from the file
unsigned TrainingData::getTargetOutputs(vector<double> &targetOutputVals)
{
    targetOutputVals.clear();

    string line;
    getline(m_trainingDataFile, line);
    stringstream ss(line);

    string label;
    ss>> label;
    if (label.compare("out:") == 0) {
        double oneValue;
        while (ss >> oneValue) {
            targetOutputVals.push_back(oneValue);
        }
    }

    return targetOutputVals.size();
}


vector<vector<double>> TrainingData::readFileMinMaxFeatureScaling() {
	vector<vector<double>> weight;
	string fileName = "DataSet\\bestWeight.txt";

	ifstream m_weightFile;
	m_weightFile.open(fileName.c_str());

	string line;
	getline(m_weightFile, line);
	stringstream ss(line);

	string top;
	ss >> top;
	double oneValue;
	vector<double> topology;
	while (ss >> oneValue) {
		topology.push_back(oneValue);
	}
	weight.push_back(topology);


	getline(m_weightFile, line);
	stringstream ss2(line);
	vector<double> weightsvector;
	while (ss2 >> oneValue) {
		weightsvector.push_back(oneValue);
	}
	weight.push_back(weightsvector);
	return weight;
}

struct Connection
{
    double weight;
    double deltaWeight;
};


class Neuron;

typedef vector<Neuron> Layer;

// ****************** class Neuron ******************
class Neuron
{
public:
    Neuron(unsigned numOutputs, unsigned myIndex);
    void setOutputVal(double val) { m_outputVal = val; }
    double getOutputVal(void) const { return m_outputVal; }
    void feedForward(const Layer &prevLayer);
    void calcOutputGradients(double targetVal);
    void calcHiddenGradients(const Layer &nextLayer);
    void updateInputWeights(Layer &prevLayer);
	double getWeight(int index) { return m_outputWeights[index].weight; };
	void setWeight(int index, double val) { m_outputWeights[index].weight = val; };

private:
    static double eta;   // [0.0..1.0] overall net training rate
    static double alpha; // [0.0..n] multiplier of last weight change (momentum)
    static double transferFunction(double x);
    static double transferFunctionDerivative(double x);
    static double randomWeight(void) { return rand() / double(RAND_MAX); }
    double sumDOW(const Layer &nextLayer) const;
    double m_outputVal;
	double m_inputVal;
    vector<Connection> m_outputWeights;
    unsigned m_myIndex;
    double m_gradient;
};

double Neuron::eta = 0.4;    // overall net learning rate, [0.0..1.0]
double Neuron::alpha = 0.0;   // momentum, multiplier of last deltaWeight, [0.0..1.0]


void Neuron::updateInputWeights(Layer &prevLayer)
{
    // The weights to be updated are in the Connection container
    // in the neurons in the preceding layer

    for (unsigned n = 0; n < prevLayer.size(); ++n) {
        Neuron &neuron = prevLayer[n];
        double oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;

        double newDeltaWeight =
                // Individual input, magnified by the gradient and train rate:
                eta
                * neuron.getOutputVal()
                * m_gradient
                // Also add momentum = a fraction of the previous delta weight;
                + alpha
                * oldDeltaWeight;

        neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
        neuron.m_outputWeights[m_myIndex].weight -= newDeltaWeight;
    }
}

double Neuron::sumDOW(const Layer &nextLayer) const
{
    double sum = 0.0;

    // Sum our contributions of the errors at the nodes we feed.

    for (unsigned n = 0; n < nextLayer.size() - 1; ++n) {
        sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
    }

    return sum;
}

void Neuron::calcHiddenGradients(const Layer& nextLayer)
{
	double dow = sumDOW(nextLayer);
	m_gradient = dow * Neuron::transferFunctionDerivative(m_inputVal); 
}

void Neuron::calcOutputGradients(double targetVals)
{
	double delta = -(targetVals - m_outputVal);
	m_gradient = delta * Neuron::transferFunctionDerivative(m_inputVal);
}

double Neuron::transferFunction(double x)
{
    // sigmoid
	// 1 / (1 + e^-x)
	return 1 / (1 + exp(-x));
}

double Neuron::transferFunctionDerivative(double x)
{
    // tanh derivative
	//return 1.0 - tanh(x) * tanh(x);
	// sigmoid derivative
	// sigmoid(x)[1-sigmoid(x)]
	return transferFunction(x) * (1 - transferFunction(x));
}

void Neuron::feedForward(const Layer &prevLayer)
{
    double sum = 0.0;

    // Sum the previous layer's outputs (which are our inputs)
    // Include the bias node from the previous layer.

    for (unsigned n = 0; n < prevLayer.size(); ++n) {
        sum += prevLayer[n].getOutputVal() *
                prevLayer[n].m_outputWeights[m_myIndex].weight;
    }

	m_inputVal = sum;
	m_outputVal = Neuron::transferFunction(sum);
}

Neuron::Neuron(unsigned numOutputs, unsigned myIndex)
{
    for (unsigned c = 0; c < numOutputs; ++c) {
        m_outputWeights.push_back(Connection());
        m_outputWeights.back().weight = randomWeight();
    }

    m_myIndex = myIndex;
}


// ****************** class Net ******************
class Net
{
public:
    Net(const vector<unsigned> &topology);
	vector<double> saveWeight();
    void feedForward(const vector<double> &inputVals);
    void backProp(const vector<double> &targetVals);
    void getResults(vector<double> &resultVals) const;
	void resetRecentAverageError(void) { m_recentAverageError = 0; }
    double getRecentAverageError(void) const { return m_recentAverageError; }
	void setWeightNet(vector<double> weights);

private:
    vector<Layer> m_layers; // m_layers[layerNum][neuronNum]
    double m_error;
    double m_recentAverageError;
    static double m_recentAverageSmoothingFactor;
};

double Net::m_recentAverageSmoothingFactor = 300.0; // Number of training samples to average over


void Net::setWeightNet(vector<double> weights) {
	int w = 0;
	for (unsigned layerNum = 0; layerNum < m_layers.size() - 1; ++layerNum) {
		for (unsigned neuronNum = 0; neuronNum < m_layers[layerNum].size(); ++neuronNum) {
			for (unsigned n = 0; n < m_layers[layerNum + 1].size() - 1; ++n) {
				m_layers[layerNum][neuronNum].setWeight(n, weights[w]);
				++w;
			}
		}
	}
}

void Net::getResults(vector<double> &resultVals) const
{
    resultVals.clear();

    for (unsigned n = 0; n < m_layers.back().size() - 1; ++n) {
        resultVals.push_back(m_layers.back()[n].getOutputVal());
    }
}

void Net::backProp(const vector<double> &targetVals)
{
    // Calculate overall net error (RMS of output neuron errors)

    Layer &outputLayer = m_layers.back();
    m_error = 0.0;

    for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
        double delta = targetVals[n] - outputLayer[n].getOutputVal();
        m_error += delta * delta;
    }
    m_error /= outputLayer.size() - 1; // get average error squared
    m_error = sqrt(m_error); // RMS

    // Implement a recent average measurement

    m_recentAverageError =
            (m_recentAverageError * m_recentAverageSmoothingFactor + m_error)
            / (m_recentAverageSmoothingFactor + 1.0);

    // Calculate output layer gradients

    for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
        outputLayer[n].calcOutputGradients(targetVals[n]);
    }

    // Calculate hidden layer gradients

    for (unsigned layerNum = m_layers.size() - 2; layerNum > 0; --layerNum) {
        Layer &hiddenLayer = m_layers[layerNum];
        Layer &nextLayer = m_layers[layerNum + 1];

        for (unsigned n = 0; n < hiddenLayer.size(); ++n) {
            hiddenLayer[n].calcHiddenGradients(nextLayer);
        }
    }

    // For all layers from outputs to first hidden layer,
    // update connection weights

    for (unsigned layerNum = m_layers.size() - 1; layerNum > 0; --layerNum) {
        Layer &layer = m_layers[layerNum];
        Layer &prevLayer = m_layers[layerNum - 1];

        for (unsigned n = 0; n < layer.size() - 1; ++n) {
            layer[n].updateInputWeights(prevLayer);
        }
    }
}

void Net::feedForward(const vector<double> &inputVals)
{
    assert(inputVals.size() == m_layers[0].size() - 1);

    // Assign (latch) the input values into the input neurons
    for (unsigned i = 0; i < inputVals.size(); ++i) {
        m_layers[0][i].setOutputVal(inputVals[i]);
    }

    // forward propagate
    for (unsigned layerNum = 1; layerNum < m_layers.size(); ++layerNum) {
        Layer &prevLayer = m_layers[layerNum - 1];
        for (unsigned n = 0; n < m_layers[layerNum].size() - 1; ++n) {
            m_layers[layerNum][n].feedForward(prevLayer);
        }
    }
}

Net::Net(const vector<unsigned> &topology)
{
    unsigned numLayers = topology.size();
    for (unsigned layerNum = 0; layerNum < numLayers; ++layerNum) {
        m_layers.push_back(Layer());
        unsigned numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1];

        // We have a new layer, now fill it with neurons, and
        // add a bias neuron in each layer.
        for (unsigned neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum) {
            m_layers.back().push_back(Neuron(numOutputs, neuronNum));
        }

        // Force the bias node's output to 1.0 (it was the last neuron pushed in this layer):
        m_layers.back().back().setOutputVal(1.0);
    }
}

vector<double> Net::saveWeight()
{
	vector<double> weights;
	for (unsigned layerNum = 0; layerNum < m_layers.size() - 1; ++layerNum) {
		for (unsigned neuronNum = 0; neuronNum < m_layers[layerNum].size(); ++neuronNum) {
			for (unsigned n = 0; n < m_layers[layerNum + 1].size() - 1; ++n) {
				weights.push_back(m_layers[layerNum][neuronNum].getWeight(n));
			}
		}
	}
	return weights;
}


void showVectorVals(string label, vector<double>& v)
{
	cout << label << " ";
	for (unsigned i = 0; i < v.size(); ++i) {
		cout << v[i] << " ";
	}

	cout << endl;
}

void saveWeightFile(vector<double> weights, vector<unsigned> topology) {

	string FileData = "";
	//Get all the weights and save them into a file
	FileData += "topology: ";
	for (int i = 0; i < topology.size(); ++i)
		FileData += to_string(topology[i]) + " ";

	FileData += "\n";
	for (int n = 0; n < weights.size(); ++n)
		FileData += to_string(weights[n]) + " ";

	ofstream m_minMaxFile;

	m_minMaxFile.open("DataSet\\bestWeight.txt", fstream::in | fstream::out | fstream::trunc);

	m_minMaxFile << FileData;

	m_minMaxFile.close();
}


void TestMethod() {
	string option = "";
	system("cls");

	//Load the file
	TrainingData trainData("DataSet\\testDataSetNormalized.txt");

	//Define the topology
	vector<unsigned> topology;
	vector<double> finalWeights;
	trainData.getTopology(topology);

	//Read the weights 
	vector<vector<double>> weights = trainData.readFileMinMaxFeatureScaling();

	//Set the weights
	Net myNet(topology);
	myNet.setWeightNet(weights[1]);
	vector<double> inputVals, targetVals, resultVals;

	int countAccuracy = 0, countDataSet = 0;
	while (!trainData.isEof()) {
		cout << endl;

		// Get new input data and feed it forward
		if (trainData.getNextInputs(inputVals) != topology[0]) {
			break;
		}
		showVectorVals("Inputs:", inputVals);
		myNet.feedForward(inputVals);

		// Collect the net's actual output results
		myNet.getResults(resultVals);
		showVectorVals("Outputs:", resultVals);

		// Show the target output vals
		trainData.getTargetOutputs(targetVals);
		showVectorVals("Targets:", targetVals);
		assert(targetVals.size() == topology.back());

		//Count the amount of correct outputs
		++countDataSet;
		if (resultVals[0] > 0.5 && targetVals[0] == 1)
			++countAccuracy;
		else if (resultVals[0] < 0.5 && targetVals[0] == 0)
			++countAccuracy;

		// Report how well the training is working, average over recent samples:
	}
	cout << endl << "Correct outputs of test: " << countAccuracy << " of " << countDataSet << endl;

	cin >> option;
}


void TrainingMethod() {

	string option = "";
	while (option != "y" && option != "n") {
		cout << "Please, inform if you want to normalize the data.\n(y/n)\n" << endl;
		cin >> option;
	}
	//If is necessary, we can normalize the data applying the MinMaxFeatureScaling
	if (option == "y") {
		string trainningFileName = "DataSet\\trainingDataSet.txt",
			finalTrainningFileName = "DataSet\\trainingDataSetNormalized.txt",
			testFileName = "DataSet\\testDataSet.txt",
			finalTestFileName = "DataSet\\testDataSetNormalized.txt";

		MinMaxFeatureScaling minMaxFeatureScaling(trainningFileName, finalTrainningFileName, testFileName, finalTestFileName);

		vector<int> topologyEntry;
		while (option != "f" || topologyEntry.size() < 3) {
			cout << endl << "Inform the numbers of neurons in the number " << topologyEntry.size() + 1 << " layer.\n ('f') to finish." << endl;
			cin >> option;
			std::string::const_iterator it = option.begin();
			while (it != option.end() && std::isdigit(*it)) ++it;
			if (!option.empty() && it == option.end()) {
				stringstream geek(option);
				int top = 0;
				geek >> top;
				topologyEntry.push_back(top);
			}
		}
		//Overwrite the old file for a new one with topology - Training data set
		std::ofstream outputFile("DataSet\\trainingDataSetNormalized2.txt");
		std::ifstream inputFile("DataSet\\trainingDataSetNormalized.txt");
		//std::string tempString;

		//Write the topology
		string FileData = "topology:";
		for (int i = 0; i < topologyEntry.size(); i++)
			FileData += " " + to_string(topologyEntry[i]);

		//Copy the data to the file
		outputFile << FileData + "\n";

		outputFile << inputFile.rdbuf();

		inputFile.close();
		outputFile.close();

		//Replaces the file for a new one
		std::remove("DataSet\\trainingDataSetNormalized.txt");
		std::rename("DataSet\\trainingDataSetNormalized2.txt", "DataSet\\trainingDataSetNormalized.txt");

		//Overwrite the old file for a new one with topology - Test data set
		std::ofstream outputFile2("DataSet\\testDataSetNormalized2.txt");
		std::ifstream inputFile2("DataSet\\testDataSetNormalized.txt");

		FileData = "topology:";
		for (int i = 0; i < topologyEntry.size(); i++)
			FileData += " " + to_string(topologyEntry[i]);

		//Copy the data to the file
		outputFile2 << FileData + "\n";

		outputFile2 << inputFile2.rdbuf();

		inputFile2.close();
		outputFile2.close();

		//Replaces the file for a new one
		std::remove("DataSet\\testDataSetNormalized.txt");
		std::rename("DataSet\\testDataSetNormalized2.txt", "DataSet\\testDataSetNormalized.txt");
	}

	//Do the actual training
	int times = 0;
	do {
		system("cls");

		TrainingData trainData("DataSet\\trainingDataSetNormalized.txt");

		// e.g., { 3, 2, 1 }
		vector<unsigned> topology;
		vector<double> finalWeights;
		trainData.getTopology(topology);

		Net myNet(topology);

		vector<double> inputVals, targetVals, resultVals;
		int bestTime = 1;
		double minorAverageError = 1.0;

		cout << endl << "Time " << times << endl;

		int countAccuracy = 0, countDataSet = 1;
		//Do while the average error is higher and do not have 100% accuracy in the training dataset
		while (myNet.getRecentAverageError() >  0.0005 || countAccuracy < countDataSet) {
			system("cls");
			times++;
			myNet.resetRecentAverageError();
			TrainingData trainData("DataSet\\trainingDataSetNormalized.txt");
			trainData.popTopology();
			cout << endl << "Time " << times;

			countAccuracy = 0;
			countDataSet = 0;

			while (!trainData.isEof()) {
				// Get new input data and feed it forward:
				if (trainData.getNextInputs(inputVals) != topology[0]) {
					break;
				}

				//showVectorVals(": Inputs:", inputVals);
				myNet.feedForward(inputVals);

				// Collect the net's actual output results:
				myNet.getResults(resultVals);
				//showVectorVals("Outputs:", resultVals);

				// Train the net what the outputs should have been:
				trainData.getTargetOutputs(targetVals);
				//showVectorVals("Targets:", targetVals);
				assert(targetVals.size() == topology.back());

				//Count the amount of correct outputs
				++countDataSet;
				if (resultVals[0] > 0.5 && targetVals[0] == 1)
					++countAccuracy;
				else if( resultVals[0] < 0.5 && targetVals[0] == 0)
					++countAccuracy;

				myNet.backProp(targetVals);

			}
			// Report how well the training is working, average over recent samples:
			cout << endl << "Net recent average error: "
				<< myNet.getRecentAverageError();
			
			if (myNet.getRecentAverageError() < minorAverageError) {
				minorAverageError = myNet.getRecentAverageError();
				bestTime = times;

				finalWeights = myNet.saveWeight();
			}

			//cout << endl << "Correct outputs of backpropagation: " << countAccuracy << " of " << countDataSet << endl;
		}

		saveWeightFile(finalWeights, topology);
		cout << endl << "Best Time: " << bestTime;
		cout << endl << "Best Average Error: " << minorAverageError;
		cout << endl << "Done" << endl;

		option = "";

		//Show results with a test
		TestMethod();


		while (option != "n" && option != "y") {
			cout << endl << endl << "Repeat the process?\n (y/n)";
			cin >> option;
		}
	}while (option != "n");

}

int main()
{
	string option = "";
	while (option != "3")
	{
		system("cls");
		cout << "(1) Start training, (2) Start test, (3) Exit." << endl;
		cin >> option;
		if (option == "1")
			TrainingMethod();
		else if (option == "2")
			TestMethod();
	}

}

