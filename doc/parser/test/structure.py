import unittest

import helpers.test_framework as test_framework
import helpers.compare as compare

class StructureTestCase(unittest.TestCase):
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
        filename_generated = self.docuFolder + "/structure/index.md"
        filename_reference = self.refFolder + "/structure/index.md"
        
        self.assertTrue(\
            *compare.compare_fileheader(filename_generated, filename_reference))
            
        members_to_test = [
            "Enumerations",
            "Type Definitions",
            "Variables",
            "Functions"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 2,\
                filename_generated, filename_reference))
                

    def test_enums(self):
        filename_generated = self.docuFolder + "/structure/index.md"
        filename_reference = self.refFolder + "/structure/index.md"

        members_to_test = [
            "structure::normal_enum",
            "structure::class_enum",
            "structure::class_enum_inheritance",
            "structure::@0"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3,\
                filename_generated, filename_reference))
                

    def test_typedefs(self):
        filename_generated = self.docuFolder + "/structure/index.md"
        filename_reference = self.refFolder + "/structure/index.md"
        self.assertTrue(\
            *compare.compare_documentation("structure::this_is_a_int", 3,\
            filename_generated, filename_reference))

    def test_variables(self):
        filename_generated = self.docuFolder + "/structure/index.md"
        filename_reference = self.refFolder + "/structure/index.md"
        
        members_to_test = [
            "structure::public_variable",
            "structure::protected_variable",
            "structure::private_variable",
            "structure::private_variable_with_value"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3,\
                filename_generated, filename_reference))


    def test_functions(self):
        filename_generated = self.docuFolder + "/structure/index.md"
        filename_reference = self.refFolder + "/structure/index.md"
        
        members_to_test = [
            "structure::function_without_parameters",
            "structure::function_set_to_default",
            "structure::function_with_one_argument",
            "structure::function_with_in_argument",
            "structure::function_with_default_value_argument",
            "structure::function_with_out_argument",
            "structure::function_with_two_arguments",
            "structure::function_with_return_value",
            "structure::const_function",
            "structure::function_with_template",
            "structure::function_with_exception",
            "structure::function_with_retval"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3, \
                filename_generated, filename_reference))


    def test_links(self):
        filename_generated = self.docuFolder + "/structure/index.md"
        filename_reference = self.refFolder + "/structure/index.md"
        # self.assertTrue(*compare.checkLinks(filename_generated, filename_reference))

    def test_class(self):
        # upper level:
        filename_generated = self.docuFolder + "/structure/index.md"
        filename_reference = self.refFolder + "/structure/index.md"
        self.assertTrue(\
            *compare.compare_documentation("Classes", 2,\
            filename_generated, filename_reference))        
        
        # the class itself:
        filename_generated = self.docuFolder + "/structure/class.md"
        filename_reference = self.refFolder + "/structure/class.md"


        members_to_test = [
            "Classes",
            "Private Types",
            "Private Attributes",
            "Private Functions"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 2, \
                filename_generated, filename_reference))

        members_to_test = [
            "structure::class::enum_inside_class",
            "structure::class::this_is_a_int",
            "structure::class::class_member_variable",
            "structure::class::class_member_function"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3, \
                filename_generated, filename_reference))
                
        self.assertTrue(\
            *compare.compare_fileheader(filename_generated, filename_reference))
                
        filename_generated = self.docuFolder + "/structure/class/class_inside_class.md"
        filename_reference = self.refFolder + "/structure/class/class_inside_class.md"

        self.assertTrue(\
            *compare.compare_fileheader(filename_generated, filename_reference))
            
        filename_generated = self.docuFolder + "/structure/struct.md"
        filename_reference = self.refFolder + "/structure/struct.md"

        self.assertTrue(\
            *compare.compare_fileheader(filename_generated, filename_reference))

        self.assertTrue(\
        *compare.compare_documentation("structure::struct::element_in_struct ", 3, \
        filename_generated, filename_reference))
            
        filename_generated = self.docuFolder + "/structure/class_with_template.md"
        filename_reference = self.refFolder + "/structure/class_with_template.md"

        self.assertTrue(\
            *compare.compare_fileheader(filename_generated, filename_reference))
            
        self.assertTrue(\
            *compare.compare_documentation("Template Parameters", 2, \
            filename_generated, filename_reference))
                

    def test_function_only_namespace(self):
        # upper level:
        filename_generated = self.docuFolder + "/structure/index.md"
        filename_reference = self.refFolder + "/structure/index.md"
        self.assertTrue(\
            *compare.compare_documentation("Namespaces", 2,\
            filename_generated, filename_reference))        
        
        # the namespace itself:
        filename_generated = self.docuFolder + \
            "/structure/function_only_namespace/index.md"
        filename_reference = self.refFolder + \
            "/structure/function_only_namespace/index.md"
            
        self.assertTrue(\
            *compare.compare_documentation("Functions", 2,\
            filename_generated, filename_reference))   

        members_to_test = [
            "structure::function_only_namespace::function2",
            "structure::function_only_namespace::function1"
            ]

        for member in members_to_test:
            self.assertTrue(\
                *compare.compare_documentation(member, 3, \
                filename_generated, filename_reference))
                
        self.assertTrue(\
            *compare.compare_fileheader(filename_generated, filename_reference))

        # self.assertTrue(*compare.checkLinks(filename_generated, filename_reference))


    def test_inner_namespace(self):
        # upper level:
        filename_generated = self.docuFolder + "/structure/index.md"
        filename_reference = self.refFolder + "/structure/index.md"
        self.assertTrue(\
            *compare.compare_documentation("Namespaces", 2,\
            filename_generated, filename_reference))           
        
        # the namespace itself:
        filename_generated = self.docuFolder + "/structure/inner_namespace/index.md"
        filename_reference = self.refFolder + "/structure/inner_namespace/index.md"

        self.assertTrue(\
            *compare.compare_documentation("Classes", 2,\
            filename_generated, filename_reference))
                        
        self.assertTrue(\
            *compare.compare_fileheader(filename_generated, filename_reference))

        filename_generated = self.docuFolder + "/structure/inner_namespace/inner_class.md"
        filename_reference = self.refFolder + "/structure/inner_namespace/inner_class.md"
        
        self.assertTrue(\
            *compare.compare_fileheader(filename_generated, filename_reference))

