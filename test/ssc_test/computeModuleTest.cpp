#include "simulation_test_info.h"
#include "computeModuleTest.h"

/// Run all tests in TestResult
TEST_P(computeModuleTest, RunSimulationTest) {
	int n = table_->getNumResult();
	bool boolCompute = compute();
	for (int i = 0; i < n; i++) {
		const TestResult* testResult = &(table_->getResult())[i];
		if (testResult->testType == ERR) {
			ASSERT_FALSE(boolCompute);
			return;
		}
		ASSERT_TRUE(boolCompute) << table_->getCMODType() << "-" << table_->name << ": did not compute.";
		ssc_number_t actualResult = 0.0;
		std::stringstream ss(testResult->sscVarName);
		std::string varName, index;
		size_t varIndex;
		// get data as element in array or as a single-value
		getline(ss, varName, '[');
		if (varName.length() < strlen(testResult->sscVarName.c_str())) {
			getline(ss, index, ']');
			varIndex = (int)atof(index.c_str());
			actualResult = ssc_data_get_array(data_, varName.c_str(), nullptr)[varIndex];
		}
		else {
			ssc_data_get_number(data_, testResult->sscVarName.c_str(), &actualResult);
		}
		std::stringstream failureMsg;
		failureMsg << table_->getCMODType() << "-" << table_->name << ":\n\t" << testResult->sscVarName << " failed. Actual/Expected: " << std::to_string(actualResult) << " / " << std::to_string(testResult->expectedResult);
		if (testResult->testType == EQ || testResult->testType == TF) {
			EXPECT_EQ(actualResult, testResult->expectedResult) << failureMsg.str();
		}
		else if (testResult->testType == NR) {
			EXPECT_NEAR(actualResult, testResult->expectedResult, testResult->errorBound*testResult->expectedResult) << failureMsg.str();
		}
		else if (testResult->testType == GT) {
			EXPECT_GT(actualResult, testResult->expectedResult) << failureMsg.str();
		}
		else if (testResult->testType == LT) {
			EXPECT_LT(actualResult, testResult->expectedResult) << failureMsg.str();
		}
		else if (testResult->testType == TF) {
			if ((bool)testResult->expectedResult == true) EXPECT_TRUE(actualResult) << failureMsg.str();
			else EXPECT_FALSE(actualResult) << failureMsg.str();
		}
		else if (testResult->testType == SIZE) {
			int siz = 0;
			ssc_data_get_array(data_, varName.c_str(), &siz);
			EXPECT_NEAR(siz, testResult->expectedResult, testResult->errorBound*testResult->expectedResult) << failureMsg.str();
		}
		else {
			std::cout << table_->getCMODType() << "-" << table_->name << ":\n\t" << testResult->sscVarName << " test type unknown.\n";
			EXPECT_TRUE(0);
		}
	}
}
