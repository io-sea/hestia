import unittest

import helpers.test_framework as test_framework
import helpers.compare as compare

class ReferenceTestCase(unittest.TestCase):
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
        filename_generated = self.docuFolder + "/reference/index.md"
        filename_reference = self.refFolder + "/reference/index.md"
        
        members_to_test = [
            "Classes",
            "Enumerations",
            "Type Definitions",
            "Variables",
            "Functions"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 2,\
                filename_generated, filename_reference))
                

    def test_typical_references(self):
        filename_generated = self.docuFolder + "/reference/index.md"
        filename_reference = self.refFolder + "/reference/index.md"

        members_to_test = [
            "reference::reference_to_class",
            "reference::reference_to_function",
            "reference::reference_to_variable",
            "reference::reference_to_enum",
            "reference::reference_to_typedef",
            "reference::implicit_reference_to_other_file",
            "reference::explicit_reference_to_other_file"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3, \
                filename_generated, filename_reference))
                
                
    def test_copydoc(self):
        filename_generated = self.docuFolder + "/reference/index.md"
        filename_reference = self.refFolder + "/reference/index.md"

        members_to_test = [
            "reference::copydoc_to_class",
            "reference::copydoc_to_function",
            "reference::copydoc_to_variable",
            "reference::copydoc_to_typedef"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3, \
                filename_generated, filename_reference))
