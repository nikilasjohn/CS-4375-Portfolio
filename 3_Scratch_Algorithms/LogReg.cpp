#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <chrono>
using namespace std;

vector<double> sigmoid(double *dataWeight, int numRows) {

	vector<double> sigmoids(numRows);

	for (int i = 0; i < numRows; i++) {
		sigmoids[i] = 1 /  (1 + exp(-dataWeight[i]));
	}

	return sigmoids;
}


int main(int argc, char** argv)
{
	ifstream inFS;
	string line;
	string numObs_in, pClass_in, survived_in, sex_in, age_in;
	vector<int> numObs;
	vector<int> pClass;
    vector<int> survived;
    vector<int> sex;
    vector<int> age;

	cout << "Opening file titanic_project.csv" << endl;
	inFS.open("titanic_project.csv");
	if (!inFS.is_open()) {
		cout << "Could not open file titanic_project.csv" << endl;
		return 1;
	}
    string header;
    getline(inFS, header);
    while (inFS.good()) {
		getline(inFS, numObs_in, ',');
        getline(inFS, pClass_in, ',');
        getline(inFS, survived_in, ',');
        getline(inFS, sex_in, ',');
		getline(inFS, age_in, '\n');

		numObs.push_back(stoi(numObs_in));
		pClass.push_back(stoi(pClass_in));
        survived.push_back(stoi(survived_in));
        sex.push_back(stoi(sex_in));
        age.push_back(stoi(age_in));
	}

	// Data fetching works

	// Starts timer 
	chrono::steady_clock::time_point start = chrono::steady_clock::now();

	double weights[2][1] = {{1}, {1}};

	int labels[800];
	for (int i = 0; i < 800; i++) {
		labels[i] = survived.at(i);
	}

	double data_matrix[800][2];

	for (int i = 0; i < 800; i++) {
		for (int j = 0; j < 2; j++) {
			if (j == 0) {
				data_matrix[i][j] = 1;
			}
			else {
				data_matrix[i][j] = sex.at(i);
			}
		}
	}
	// The setup for the matrices is done

	// Lines 37-47
	double learning_rate = 0.001;

	// edit the conditional statement to increase the amount of loops
	// Previously, it ran 500,000 times
	for (int i = 0; i < 100; i++) {
        
		double dataWeight[800] = {};
		// Matrix multiply (data_matrix * weights)
		for(int r1 = 0; r1 < 800; r1++){
			for(int c2 = 0; c2 < 1; c2++) {
				for(int c1 = 0; c1 < 2; c1++) {
                		dataWeight[r1] += data_matrix[r1][c1] * weights[c1][0];
           		}
			}
		}
		// Correct

		vector<double> prob_vector;

		// Correct
		prob_vector = sigmoid(dataWeight, 800);

		vector<double> error(800);
		for (int r = 0; r < error.size(); r++) {
			error.at(r) = labels[r] - prob_vector.at(r);
		}
		// Correct

		double transpose[2][800] = {};
		// Find transpose of data_matrix
		for (int r = 0; r < 800; r++) {
			for (int c = 0; c < 2; c++) {
				transpose[c][r] = data_matrix[r][c];
			}
		}

		double dataError[2][1] = {};
		// Matrix multiply (transpose of data_matrix * error) 
		// 2x800 * 800x1 = 2x1
		for (int r = 0; r < 2; r++) {
			for (int c = 0; c < 1; c++) {
				for (int r2 = 0; r2 < 800; r2++) {
					dataError[r][c] += transpose[r][r2] * error.at(r2);
				}
			}
		}
		// End matrix multiply

		// Calculate the weights
		for (int j = 0; j < 2; j++) {
			weights[j][0] += learning_rate * dataError[j][0];
		}
	}

	cout << "\nWeights:" << endl;
	for (int i = 0; i < 2; i++) {
		cout << weights[i][0] << endl;
	}
	// This part is done
	cout << endl;

	// Lines 51-60

	double test_matrix[200][2];
	int test_labels[200];

	// This matrix is flipped, meaning the last element on the dataframe
	// Is the first element on this one
	int thousand = 1045;
	for (int i = 0; i < 200; i++) {
		for (int j = 0; j < 2; j++) {
			if (j == 0) {
				test_matrix[i][j] = 1;
			}
			else {
				test_matrix[i][j] = sex.at(thousand);
			}
		}
		test_labels[i] = survived.at(thousand);
		thousand--;
	}
	// The setup for the matrices is done

	// Matrix Multiplication (test_matrix * weights)
	// 200x2 * 2x1 = 200x1
	double predicted[200];
	for (int r = 0; r < 200; r++) {
		for (int c = 0; c < 1; c++) {
			for (int r2 = 0; r2 < 2; r2++) {
				predicted[r] += test_matrix[r][r2] * weights[r2][0];
			}
		}
	}
	// End matrix multiply

	double probabilities[200];

	for (int i = 0; i < 200; i++) {
		probabilities[i] = exp(predicted[i] / (1 + exp(predicted[i])));
	} 
	// Correct

	// Resume on line 57
	int predictions[200];
	for (int i = 0; i < 200; i++) {
		if(probabilities[i] > 0.5) {
			predictions[i] = 1;
		}
		else {
			predictions[i] = 0;
		}
	}

	int trueN = 0;
	int trueP = 0;
	int falseN = 0;
	int falseP = 0;

	for (int i = 0; i < 200; i++) {
		if (predictions[i] == test_labels[i]) {
			if (predictions[i] == 0) {
				trueN++;
			}
			else if (predictions[i] == 1) {
				trueP++;
			}
		}
		else {
			if (predictions[i] == 0) {
				falseN++;
			}
			else if (predictions[i] == 1) {
				falseP++;
			}
		}
	}

	// Ends the timer
	chrono::steady_clock::time_point end = chrono::steady_clock::now(); 

	double accuracy = (trueN + trueP) / (double)(trueN + trueP + falseN + falseP);

	cout << "Accuracy: " << accuracy << endl;

	double sensitivity = (trueP) / (double)(trueP + falseN);

	cout << "Sensitivity: " << sensitivity << endl;

	double specificity = (trueN) / (double)(trueN + falseP);

	cout << "Specificity: " << specificity << endl;

	cout << "Time taken (microseconds): " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;

	return 0;
}