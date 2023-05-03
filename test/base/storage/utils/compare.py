# Copyright (c) 2020, Irish Centre for High End Computing (ICHEC), NUI Galway
# Authors:
#     Ciarán O'Rourke <ciaran.orourke@ichec.ie>,
#     Sophie Wenzel-Teuber <sophie.wenzel-teuber@ichec.ie>
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

import os


def files_are_equal(filename1, filename2):
    if not os.path.isfile(filename1):
        return False, "Error: File not found: " + filename1
    if not os.path.isfile(filename2):
        return False, "Error: File not found: " + filename2

    file1 = open(filename1, encoding="utf-8")
    data1 = file1.read()
    file1.close()

    file2 = open(filename2, encoding="utf-8")
    data2 = file2.read()
    file2.close()

    if data1 != data2:
        return False, "Error: " + filename1 + " and " + filename2 + "do not contain the same data"

    return True, ""
