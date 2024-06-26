import logging

def setup_default_logging(work_dir):
    logging.basicConfig(filename=work_dir, filemode='w', 
            format='%(asctime)s - %(message)s', datefmt='%d-%b-%y %H:%M:%S',
            level=logging.INFO)
    #console logging