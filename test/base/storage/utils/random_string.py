# Copyright (c) 2020, Irish Centre for High End Computing (ICHEC), NUI Galway
# Authors:
#     Ciarán O'Rourke <ciaran.orourke@ichec.ie>,
#     Sophie Wenzel-Teuber <sophie.wenzel-teuber@ichec.ie>
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

import random
import string


def random_string(prefix='', length=16):
    return prefix + ''.join([
        random.choice(string.ascii_letters + string.digits)
        for i in range(length)
    ])


def random_map(size, length):
    dictionary = {random_string(): random_string()}

    for i in range(size - 1):
        dictionary.update({random_string(): random_string()})

    return dictionary
