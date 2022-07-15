import unittest

import helpers.test_framework as test_framework
import helpers.compare as compare

class FeatureTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.docuFolder = test_framework.create_documentation()
        if not cls.docuFolder:
            cls.fail("Could not create documentation!")

        cls.refFolder = test_framework.get_reference_docu_path()
        
    @classmethod
    def tearDownClass(cls):
        test_framework.cleanUp(cls.docuFolder)

    def test_namespace(self):
        filename_generated = self.docuFolder + "/feature/index.md"
        filename_reference = self.refFolder + "/feature/index.md"
                                
        self.assertTrue(\
            *compare.compare_documentation("Classes", 2, \
            filename_generated, filename_reference))


    def test_includes(self):
        filename_generated = self.docuFolder + "/feature/generic_include_test_class.md"
        filename_reference = self.refFolder + "/feature/generic_include_test_class.md"

        self.assertTrue(\
            *compare.compare_documentation("Includes", 2, \
            filename_generated, filename_reference))
            
        
    def test_inheritance_graph(self):
        filename_generated = self.docuFolder + "/feature/Base.md"
        filename_reference = self.refFolder + "/feature/Base.md"

        self.assertTrue(\
            *compare.compare_documentation("Inheritance", 2, \
            filename_generated, filename_reference))
            
        self.assertTrue(\
            *compare.compare_documentation("feature::Base::override_function", 3, \
            filename_generated, filename_reference))

        filename_generated = self.docuFolder + "/feature/Derived.md"
        filename_reference = self.refFolder + "/feature/Derived.md"

        self.assertTrue(\
            *compare.compare_documentation("Inheritance", 2, \
            filename_generated, filename_reference))
            
        self.assertTrue(\
            *compare.compare_documentation("feature::Derived::override_function", 3, \
            filename_generated, filename_reference))
    