import unittest

import helpers.test_framework as test_framework
import helpers.compare as compare

class DescriptionTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.docuFolder = test_framework.create_documentation()
        if not cls.docuFolder:
            cls.fail("Could not create documentation!")

        cls.refFolder = test_framework.get_reference_docu_path()
        
    @classmethod
    def tearDownClass(cls):
        test_framework.cleanUp(cls.docuFolder)

    def test_header(self):
        filename_generated = self.docuFolder + "/description/index.md"
        filename_reference = self.refFolder + "/description/index.md"

        self.assertTrue(\
            *compare.compare_documentation("Functions", 2, \
            filename_generated, filename_reference))

        
    def test_code(self):
        filename_generated = self.docuFolder + "/description/index.md"
        filename_reference = self.refFolder + "/description/index.md"

        members_to_test = [
            "description::function_with_little_code",
            "description::function_with_code_block",
            "description::function_with_code_text_mix"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3, \
                filename_generated, filename_reference))


    def test_doxygen_commands(self):
        filename_generated = self.docuFolder + "/description/index.md"
        filename_reference = self.refFolder + "/description/index.md"
            
        members_to_test = [
            "description::function_with_remark",
            "description::function_with_pre_condition",
            "description::function_with_post_condition",
            "description::function_with_warning"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3, \
                filename_generated, filename_reference))
                
    def test_todos(self):
        filename_generated = self.docuFolder + "/description/index.md"
        filename_reference = self.refFolder + "/description/index.md"
            
        members_to_test = [
            "description::function_with_todo",
            "description::function_with_todo_and_reference",
            "description::function_with_todo_and_code",
            "description::function_with_two_todos",
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3, \
                filename_generated, filename_reference))
        
        filename_generated = self.docuFolder + "/todo.md"
        filename_reference = self.refFolder + "todo.md"
        
        self.assertTrue(\
            *compare.compare_fileheader(filename_generated, filename_reference))
        

            
    def test_html_descriptions(self):
        filename_generated = self.docuFolder + "/description/index.md"
        filename_reference = self.refFolder + "/description/index.md"
        
        members_to_test = [
            "description::function_with_bold_text",
            "description::function_with_italic_text"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3, \
                filename_generated, filename_reference))

            
    def test_multiline(self):
        filename_generated = self.docuFolder + "/description/index.md"
        filename_reference = self.refFolder + "/description/index.md"
        
        members_to_test = [
            "description::function_with_long_description",
            "description::function_with_list"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3, \
                filename_generated, filename_reference))
    
    # def testLinks(self):
    #     TODO
