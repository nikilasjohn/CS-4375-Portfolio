// 1_C++_Exploration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
using namespace std;

double findSum(vector<double>);
double findMean(double, int);
double findMedian(vector<double>);
double* findRange(vector<double>);
double findCoV(vector<double>, vector<double>);
double findSqrtVar(vector<double>);
double findCorr(vector<double>, vector<double>);

int main(int argc, char** argv)
{
	ifstream inFS;
	string line;
	string rm_in, medv_in;
	vector<double> rm;
	vector<double> medv;

	cout << "Opening file Boston.csv" << endl;
	inFS.open("Boston.csv");
	if (!inFS.is_open()) {
		cout << "Could not open file Boston.csv" << endl;
		return 1;
	}

	cout << "Reading line 1" << endl;
	getline(inFS, line);
	
	cout << "heading: " << line << endl;
    
	int numObservations = 0;
	while (inFS.good()) {
		getline(inFS, rm_in, ',');
		getline(inFS, medv_in, '\n');

		rm.push_back(stof(rm_in));
		medv.push_back(stof(medv_in));

		numObservations++;
	}

	rm.resize(numObservations);
	medv.resize(numObservations);

	cout << "new length: " << rm.size() << endl;
	
	cout << "Closing file Boston.csv." << endl;
	inFS.close();

    double sumRm = findSum(rm);
    double* rangeRm = findRange(rm);
    
	cout << "\nNumber of records: " << numObservations << endl;

	cout << "\nStats for rm" << endl;
	cout << "Sum: " << sumRm << endl;
    cout << "Mean: " << findMean(sumRm, numObservations) << endl;
    cout << "Median: " << findMedian(rm) << endl;
    cout << "Range: [" << rangeRm[0] << ":" << rangeRm[1] << "]" << endl;

    double sumMed = findSum(medv);
    double* rangeMed = findRange(medv);

	cout << "\nStats for medv" << endl;
	cout << "Sum: " << sumMed << endl;
    cout << "Mean: " << findMean(sumMed, numObservations) << endl;
    cout << "Median: " << findMedian(medv) << endl;
    cout << "Range: [" << rangeMed[0] << ":" << rangeMed[1] << "]" << endl;

	cout << "\nCovariance: " << findCoV(rm, medv) << endl;
	cout << "Correlation: " << findCorr(rm, medv) << endl;

	cout << "\nProgram Terminated";

	return 0;
}

// Finds the sum of a vector
double findSum(vector<double> v) {
    double sum = 0;
    for(int i = 0; i < v.size(); i++) {
        sum += v.at(i);
    }
    return sum;
}

// Finds the mean of a vector
double findMean(double sum, int size) {
    return (sum / size);
}

// Finds the median of a vector
double findMedian(vector<double> v) {

    sort(v.begin(), v.end());

    return (v.at(v.size() / 2));
}

// Finds the range of a vector
double* findRange(vector<double> v) {
    static double range[2];
    sort(v.begin(), v.end());

    range[0] = v.at(0);
    range[1] = v.at(v.size() - 1);

    return range;
}

// Finds the covariance of two vectors
double findCoV(vector<double> v1, vector<double> v2) {
    double sum = 0;
    double meanV1 = findMean(findSum(v1), v1.size());
    double meanV2 = findMean(findSum(v2), v2.size());

    for (int i = 0; i < v1.size(); i++) {
        sum += ((v1[i] - meanV1) * (v2[i] - meanV2));
    }

    return sum / (v1.size() - 1);
}

// Finds the square root of the variance of a vector
double findSqrtVar(vector<double> v) {
    double mean = findMean(findSum(v), v.size());
    double sumSq = 0;
    for (int i = 0; i < v.size(); i++) {
        sumSq += (v[i] - mean) * (v[i] - mean);
    }
    double sqrtSumSq = sqrt(sumSq / v.size());
    return sqrtSumSq;
}

// finds the correlation of two vectors
double findCorr(vector<double> v1, vector<double> v2) {
    return (findCoV(v1, v2) / (findSqrtVar(v1) * findSqrtVar(v2)));
}