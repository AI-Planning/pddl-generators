import numpy


# A ring of small machines 2-7 with a fast connection between themselves. A super computer
# (more memory and faster cpu) is in the middle but the connection cost is high.

FILE_SIZE_DISTRIBUTION = lambda: numpy.random.random_integers(1, 5)

SERVERS = {
    "server1": {
        "capacity": 16,
        "io": 1,
        "processing_distr": (lambda: max(1, int(numpy.random.normal(10, 2)))),
    },
    "server2": {
        "capacity": 8,
        "io": 2,
        "processing_distr": (lambda: max(1, int(numpy.random.normal(20, 4)))),
    },
    "server3": {
        "capacity": 8,
        "io": 2,
        "processing_distr": (lambda: max(1, int(numpy.random.normal(20, 4)))),
    },
    "server4": {
        "capacity": 8,
        "io": 2,
        "processing_distr": (lambda: max(1, int(numpy.random.normal(20, 4)))),
    },
    "server5": {
        "capacity": 8,
        "io": 2,
        "processing_distr": (lambda: max(1, int(numpy.random.normal(20, 4)))),
    },
    "server6": {
        "capacity": 8,
        "io": 2,
        "processing_distr": (lambda: max(1, int(numpy.random.normal(20, 4)))),
    },
    "server7": {
        "capacity": 8,
        "io": 2,
        "processing_distr": (lambda: max(1, int(numpy.random.normal(20, 4)))),
    },
}


def union(dict1, dict2):
    return dict(list(dict1.items()) + list(dict2.items()))


CONNECTIONS = union(
    {(1, i): {"send": 20} for i in range(2, 8)},
    {(i, i + 1 if i < 7 else 2): {"send": 2} for i in range(2, 8)},
)
