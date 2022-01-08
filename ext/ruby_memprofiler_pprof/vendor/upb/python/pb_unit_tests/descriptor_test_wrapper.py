# Copyright (c) 2009-2021, Google LLC
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Google LLC nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL Google LLC BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from google.protobuf.internal import descriptor_test
import unittest

descriptor_test.DescriptorCopyToProtoTest.testCopyToProto_TypeError.__unittest_expecting_failure__ = True
descriptor_test.GeneratedDescriptorTest.testDescriptor.__unittest_expecting_failure__ = True
descriptor_test.MakeDescriptorTest.testCamelcaseName.__unittest_expecting_failure__ = True
descriptor_test.MakeDescriptorTest.testJsonName.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testAggregateOptions.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testComplexExtensionOptions.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testContainingServiceFixups.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testContainingTypeFixups.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testCustomOptionsCopyTo.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testDefault.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testDifferentCustomOptionTypes.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testEnumFixups.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testEnumValueName.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testFileDescriptor.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testFileDescriptorReferences.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testGetOptions.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testImmutableCppDescriptor.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testNestedOptions.__unittest_expecting_failure__ = True
descriptor_test.NewDescriptorTest.testSimpleCustomOptions.__unittest_expecting_failure__ = True

# We must skip these tests entirely (rather than running them with
# __unittest_expecting_failure__) because they error out in setUp():
#
#  TypeError: Couldn't build proto file into descriptor pool: duplicate file name (some/filename/some.proto)
#
# TODO: change to __unittest_expecting_failure__ when we have some solution for duplicated filenames
descriptor_test.DescriptorTest.__unittest_skip__ = True

if __name__ == '__main__':
  unittest.main(module=descriptor_test, verbosity=2)
