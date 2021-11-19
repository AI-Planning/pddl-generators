import numpy

FILE_SIZE_DISTRIBUTION = lambda: numpy.random.random_integers(1, 5)

SERVERS = {
    "server1": {
        "capacity": 16,
        "io": 5,
        "processing_distr": (lambda: max(1, int(numpy.random.normal(20, 4)))),
    },
    "server2": {
        "capacity": 8,
        "io": 1,
        "processing_distr": (lambda: max(1, int(numpy.random.normal(10, 2)))),
    },
    "server3": {
        "capacity": 8,
        "io": 1,
        "processing_distr": (lambda: max(1, int(numpy.random.normal(10, 2)))),
    },
}

CONNECTIONS = {(1, 2): {"send": 4}, (1, 3): {"send": 6}}
