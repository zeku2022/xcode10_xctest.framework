//
// Copyright (c) 2013-2015 Apple Inc. All rights reserved.
//
// Copyright (c) 1997-2005, Sen:te (Sente SA).  All rights reserved.
//
// Use of this source code is governed by the following license:
// 
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
// 
// (1) Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer.
// 
// (2) Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation 
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL Sente SA OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Note: this license is equivalent to the FreeBSD license.
// 
// This notice may not be removed from this file.

#import <XCTest/XCAbstractTest.h>
#import <XCTest/XCTestDefines.h>
#import <XCTest/XCTActivity.h>

NS_ASSUME_NONNULL_BEGIN

@class XCTestSuite;

#if XCT_UI_TESTING_AVAILABLE
@class XCUIElement;
#endif

/*!
 * @class XCTestCase
 * XCTestCase is a concrete subclass of XCTest that should be the override point for
 * most developers creating tests for their projects. A test case subclass can have
 * multiple test methods and supports setup and tear down that executes for every test
 * method as well as class level setup and tear down.
 *
 * To define a test case:
 *
 * ??? Create a subclass of XCTestCase.
 * ??? Implement -test methods.
 * ??? Optionally define instance variables or properties that store the state of the test.
 * ??? Optionally initialize state by overriding -setUp
 * ??? Optionally clean-up after a test by overriding -tearDown.
 *
 * Test methods are instance methods meeting these requirements:
 * ??? accepting no parameters
 * ??? returning no value
 * ??? prefixed with 'test'
 *
 * For example:

    - (void)testSomething;

 * Test methods are automatically recognized as test cases by the XCTest framework.
 * Each XCTestCase subclass's defaultTestSuite is a XCTestSuite which includes these
 * tests. Test method implementations usually contain assertions that must be verified
 * for the test to pass, for example:
 * @textblock

    @interface MathTest : XCTestCase {
    @private
        float f1;
        float f2;
    }

    - (void)testAddition;

    @end

    @implementation MathTest

    - (void)setUp
    {
        f1 = 2.0;
        f2 = 3.0;
    }

    - (void)testAddition
    {
        XCTAssertTrue (f1 + f2 == 5.0);
    }
    @end

 * @/textblock
 */
@interface XCTestCase : XCTest {
#ifndef __OBJC2__
@private
    id _internalImplementation;
#endif
}

/*!
 * @method +testCaseWithInvocation:
 */
+ (instancetype)testCaseWithInvocation:(nullable NSInvocation *)invocation;

/*!
 * @method -initWithInvocation:
 */
- (instancetype)initWithInvocation:(nullable NSInvocation *)invocation;

/*!
 * @method +testCaseWithSelector:
 */
+ (nullable instancetype)testCaseWithSelector:(SEL)selector;

/*!
 * @method -initWithSelector:
 */
- (instancetype)initWithSelector:(SEL)selector;

/*!
 * @property invocation
 * The invocation used when this test is run.
 */
@property (strong, nullable) NSInvocation *invocation;

/*!
 * @method -invokeTest
 * Invoking a test performs its setUp, invocation, and tearDown. In general this
 * should not be called directly.
 */
- (void)invokeTest;

/*!
 * @property continueAfterFailure
 * The test case behavior after a failure. Defaults to YES.
 */
@property BOOL continueAfterFailure;

/*!
 * @method -recordFailureWithDescription:inFile:atLine:expected:
 * Records a failure in the execution of the test and is used by all test assertions.
 *
 * @param description The description of the failure being reported.
 *
 * @param filePath The file path to the source file where the failure being reported
 * was encountered.
 *
 * @param lineNumber The line number in the source file at filePath where the
 * failure being reported was encountered.
 *
 * @param expected YES if the failure being reported was the result of a failed assertion,
 * NO if it was the result of an uncaught exception.
 *
 */
- (void)recordFailureWithDescription:(NSString *)description inFile:(NSString *)filePath atLine:(NSUInteger)lineNumber expected:(BOOL)expected;

/*!
 * @property testInvocations
 * Invocations for each test method in the test case.
 */
@property (class, readonly, copy) NSArray<NSInvocation *> *testInvocations;

/*!
 * @method -addTeardownBlock:
 * Registers a block to be run at the end of a test.
 *
 * Teardown blocks are executed after the current test method has returned but before
 * -tearDown is invoked.
 *
 * Registered blocks are run on the main thread but can be registered from any thread.
 * They are guaranteed to run only once, in LIFO order, and are executed serially. You
 * may register blocks during -setUp, but you may *not* register blocks during -tearDown
 * or from other teardown blocks.
 *
 * @param block A block to enqueue for future execution.
*/
- (void)addTeardownBlock:(void (^)(void))block;

#pragma mark - Measuring Performance Metrics


typedef NSString * XCTPerformanceMetric NS_EXTENSIBLE_STRING_ENUM;

/*!
 * @const XCTPerformanceMetric_WallClockTime
 * Records wall clock time in seconds between startMeasuring/stopMeasuring.
 */
XCT_EXPORT XCTPerformanceMetric const XCTPerformanceMetric_WallClockTime;

/*!
 * @property defaultPerformanceMetrics
 * The names of the performance metrics to measure when invoking -measureBlock:. Returns XCTPerformanceMetric_WallClockTime by default. Subclasses can override this to change the behavior of -measureBlock:
 */
@property (class, readonly, copy) NSArray<XCTPerformanceMetric> *defaultPerformanceMetrics;

/*!
 * @method -measureBlock:
 *
 * Call from a test method to measure resources (+defaultPerformanceMetrics) used by the
 * block in the current process.

    - (void)testPerformanceOfMyFunction {

        [self measureBlock:^{
            // Do that thing you want to measure.
            MyFunction();
        }];
    }

 * @param block A block whose performance to measure.
 */
- (void)measureBlock:(XCT_NOESCAPE void (^)(void))block;

/*!
 * @method -measureMetrics:automaticallyStartMeasuring:forBlock:
 *
 * Call from a test method to measure resources (XCTPerformanceMetrics) used by the
 * block in the current process. Each metric will be measured across calls to the block.
 * The number of times the block will be called is undefined and may change in the
 * future. For one example of why, as long as the requested performance metrics do
 * not interfere with each other the API will measure all metrics across the same
 * calls to the block. If the performance metrics may interfere the API will measure
 * them separately.
 
    - (void)testMyFunction2_WallClockTime {
        [self measureMetrics:[self class].defaultPerformanceMetrics automaticallyStartMeasuring:NO forBlock:^{

            // Do setup work that needs to be done for every iteration but you don't want to measure before the call to -startMeasuring
            SetupSomething();
            [self startMeasuring];

            // Do that thing you want to measure.
            MyFunction();
            [self stopMeasuring];

            // Do teardown work that needs to be done for every iteration but you don't want to measure after the call to -stopMeasuring
            TeardownSomething();
        }];
    }

 * Caveats:
 * ??? If YES was passed for automaticallyStartMeasuring and -startMeasuring is called
 *      anyway, the test will fail.
 * ??? If NO was passed for automaticallyStartMeasuring then -startMeasuring must be
 *      called once and only once before the end of the block or the test will fail.
 * ??? If -stopMeasuring is called multiple times during the block the test will fail.
 *
 * @param metrics An array of NSStrings (XCTPerformanceMetrics) to measure. Providing an unrecognized string is a test failure.
 *
 * @param automaticallyStartMeasuring If NO, XCTestCase will not take any measurements until -startMeasuring is called.
 *
 * @param block A block whose performance to measure.
 */
- (void)measureMetrics:(NSArray<XCTPerformanceMetric> *)metrics automaticallyStartMeasuring:(BOOL)automaticallyStartMeasuring forBlock:(XCT_NOESCAPE void (^)(void))block;

/*!
 * @method -startMeasuring
 * Call this from within a measure block to set the beginning of the critical section.
 * Measurement of metrics will start at this point.
 */
- (void)startMeasuring;

/*!
 * @method -stopMeasuring
 * Call this from within a measure block to set the ending of the critical section.
 * Measurement of metrics will stop at this point.
 */
- (void)stopMeasuring;

#pragma mark - UI Testing Support
#if XCT_UI_TESTING_AVAILABLE

/*! Adds a handler to the current context. Returns a token that can be used to unregister the handler. Handlers are invoked in the reverse order in which they are added until one of the handlers returns true, indicating that it has handled the alert.
 @param handlerDescription Explanation of the behavior and purpose of this handler, mainly used for debugging and analysis.
 @param handler Handler block for asynchronous UI such as alerts and other dialogs. Handlers should return true if they handled the UI, false if they did not. The handler is passed an XCUIElement representing the top level UI element for the alert.
 */
- (id <NSObject>)addUIInterruptionMonitorWithDescription:(NSString *)handlerDescription handler:(BOOL (^)(XCUIElement *interruptingElement))handler NS_AVAILABLE(10_11, 9_0);

/*! Removes a handler using the token provided when it was added. */
- (void)removeUIInterruptionMonitor:(id <NSObject>)monitor NS_AVAILABLE(10_11, 9_0);

#endif

@end

@interface XCTestCase (XCTestSuiteExtensions)

/*!
 * @property defaultTestSuite
 * Returns a test suite containing test cases for all of the tests in the class.
 */
@property (class, readonly) XCTestSuite *defaultTestSuite;

/*!
 * @method +setUp
 * Suite-level setup method called before the class begins to run any of its test methods or their associated
 * per-instance setUp methods.
 */
+ (void)setUp;

/*!
 * @method +tearDown
 * Suite-level teardown method called after the class has finished running all of its test methods and their
 * associated per-instance tearDown methods and teardown blocks.
 */
+ (void)tearDown;

@end

/*!
 * XCTestCase conforms to XCTActivity, allowing test attachments to be added directly from test methods.
 *
 * See XCTAttachment.h for details on how to create attachments. Once created, they can be added directly to XCTestCase:
 *
 * @textblock
 
     - (void)testFoo
     {
         XCTAttachment *attachment = ...
         [self addAttachment:attachment];
     }
 
 * @/textblock
 */
@interface XCTestCase() <XCTActivity>
@end

NS_ASSUME_NONNULL_END
