#ifndef UNIT_TEST_RUNNER_H
#define UNIT_TEST_RUNNER_H

class UnitTestRunner
{
public:
    UnitTestRunner();
    bool runAllTests();

private:
    bool runArchiveManagerTests();
    bool runTableModelTests();
    bool runArchiverPipelineTests();
};
#endif // UNIT_TEST_RUNNER_H
