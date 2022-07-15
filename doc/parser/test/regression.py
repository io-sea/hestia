import unittest

import helpers.test_framework as test_framework
import helpers.compare as compare

class RegressionTestCase(unittest.TestCase):
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
        filename_generated = self.docuFolder + "/regression/index.md"
        filename_reference = self.refFolder + "/regression/index.md"
        
        members_to_test = [
            "Type Definitions",
            "Functions"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 2,\
                filename_generated, filename_reference))
                

    def test_code_reference_mixture(self):
        filename_generated = self.docuFolder + "/regression/index.md"
        filename_reference = self.refFolder + "/regression/index.md"

        self.assertTrue(\
            *compare.compare_documentation("function_with_reference_in_code", 3, \
            filename_generated, filename_reference))
            

    def test_pointers_in_names(self):
        filename_generated = self.docuFolder + "/regression/index.md"
        filename_reference = self.refFolder + "/regression/index.md"

        self.assertTrue(\
            *compare.compare_documentation("function_with_pointers", 3, \
            filename_generated, filename_reference))
                        

    def test_references_in_names(self):
        filename_generated = self.docuFolder + "/regression/index.md"
        filename_reference = self.refFolder + "/regression/index.md"

        self.assertTrue(\
            *compare.compare_documentation("function_with_reference_in_name", 3, \
            filename_generated, filename_reference))
        
        
    def test_spaceless_templates_in_names(self):
        filename_generated = self.docuFolder + "/regression/index.md"
        filename_reference = self.refFolder + "/regression/index.md"

        self.assertTrue(\
            *compare.compare_documentation("function_with_spaceless_templates", 3, \
            filename_generated, filename_reference))
            
        
    def test_copydoc_for_function_typedef(self):
        filename_generated = self.docuFolder + "/regression/index.md"
        filename_reference = self.refFolder + "/regression/index.md"
        
        members_to_test = [
            "function_using_definition_for_copydoc",
            "function_typedef",
            "function_impl_for_using_definition"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3,\
                filename_generated, filename_reference))
    