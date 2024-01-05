import os 
import string 
import random

def id_generator(size, chars=string.ascii_uppercase + string.digits):
    return ''.join(random.choice(chars) for _ in range(size))

def generate_random_file(path, size):
    with open(path, 'wb') as f:
        f.write(os.urandom(size))

def generate_random_files(work_dir, count = 0, depth=0, total=1000, 
                          min_size=100, max_size=10000, max_depth=5):
    internal_count = count

    if depth >= max_depth or count >= total:
        return internal_count

    for idx in range(20):
        if random.randint(0, 5) == 0:
            dir_name = id_generator(random.randint(5, 10))
            os.mkdir(work_dir / dir_name)
            internal_count = generate_random_files(work_dir / dir_name,
                                                                    internal_count,
                                                                    depth+1,
                                                                    total,
                                                                    min_size,
                                                                    max_size,
                                                                    max_depth)
            if internal_count >= total:
                return internal_count
        else:
            filesize = random.randint(min_size, max_size)
            filename = id_generator(random.randint(5, 10)) + ".dat"
            generate_random_file(work_dir / filename, filesize)
            internal_count = internal_count + 1
            if internal_count >= total:
                return internal_count
    return internal_count