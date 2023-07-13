# Import packages
import csv
import os
import sys
import matplotlib.pyplot as plt
import math


class graph_node:
    id = "-"
    x = 0
    y = 0
    yaw = 0
    succ = []

    def __int__(self, id, x, y, yaw):
        self.id = id
        self.x = x
        self.y = y
        self.yaw = yaw
        self.succ = []

    def add_successor(self, succ_id):
        self.succ.append(succ_id)


def get_node_from_id(graph_nodes, node_id):
    result = []
    for node in graph_nodes:

        if node_id == node.id:
            result = node
            break

    return result


def read_trajectory_txt_file(trajectory_file_path):
    rows = []
    with open(trajectory_file_path, "r") as trajectory_file_txt:
        txt_reader = csv.reader(trajectory_file_txt, delimiter=' ')

        for row in txt_reader:
            rows.append(row)

    return rows


def read_squares_txt_file(squares_file_path):
    rows = []
    with open(squares_file_path, "r") as squares_file_txt:
        txt_reader = csv.reader(squares_file_txt, delimiter=' ')

        for row in txt_reader:
            rows.append(row)

    return rows


def trajectory_tree_plot(trajectory_rows, figpath, squares=None, nodes=None, tree_depth=None):
    if squares is None:
        squares = []
    x, y, a = [], [], []
    for item in trajectory_rows:
        if len(item) >= 3:
            x.append(float(item[0]))
            y.append(float(item[1]))
            a.append(float(item[2]) * (180.0 / math.pi))

    x_sq = []
    y_sq = []
    for item in squares:
        if len(item) == 4:
            x_sq.append([float(item[0]), float(item[1])])
            y_sq.append([float(item[2]), float(item[3])])

    fig, axs = plt.subplots(2, sharex=True)
    fig.suptitle('Trajectory; path and angle')
    axs[0].set_ylabel("Y [m]")
    axs[1].set_xlabel("X [m]")
    if len(squares) > 0:
        for i in range(len(squares)):
            x_sq_draw = [x_sq[i][0], x_sq[i][0], x_sq[i][1], x_sq[i][1], x_sq[i][0]]
            y_sq_draw = [y_sq[i][1], y_sq[i][0], y_sq[i][0], y_sq[i][1], y_sq[i][1]]
            axs[0].plot(x_sq_draw, y_sq_draw)

    nodes_enabled = False
    if nodes is not None:
        nodes_enabled = True
        for i, n in enumerate(nodes):
            axs[0].plot(n.x, n.y, linestyle="", marker="o", markersize=0.5)

            for j, s in enumerate(n.succ):
                if tree_depth is not None:
                    if j > tree_depth:
                        break
                succ_x = nodes[int(s)].x
                succ_y = nodes[int(s)].y
                axs[0].plot([n.x, succ_x], [n.y, succ_y])
    linewidth = 6 if nodes_enabled else 1
    axs[0].plot(x, y, linewidth=linewidth)
    axs[1].plot(x, a, linestyle="", marker="o", markersize=0.5)
    axs[1].set_ylabel("Degrees [º]")
    axs[0].grid()
    axs[1].grid()

    plt.savefig(figpath, dpi=300)


def ensure_path_exists(paths):
    for path in paths:
        # Ensure it exists
        if not os.path.exists(path):
            print(f"Path '{path}' does not exist in this system")
            exit()

    return


def get_vrt_viz_graph(files):
    viz_file = ""
    vrt_file = ""

    for f in files:

        if f.endswith(".viz"):
            viz_file = f
        elif f.endswith(".vrt"):
            vrt_file = f

    return [vrt_file, viz_file]


def read_graph_vertices(file):
    nodes = []
    with open(file, "r") as vrt:
        vrt_reader = csv.reader(vrt, delimiter=' ')

        for row in vrt_reader:
            node_id = row[0]
            node_x = float(row[1])
            node_y = float(row[2])
            node_yaw = float(row[3])

            new_node = graph_node()
            new_node.__int__(node_id, node_x, node_y, node_yaw)
            nodes.append(new_node)

    return nodes


def add_graph_successors(file, graph_nodes):
    with open(file, "r") as viz:
        viz_reader = csv.reader(viz, delimiter='\n')

        for i, r in enumerate(viz_reader):
            if "->" in r[0]:
                node_id = r[0].split("->")[0]
                node_succ = r[0].split("->")[1].replace(";", "").strip()

                graph_nodes[int(node_id)].add_successor(node_succ)

    return True

def main(arguments):
    # Get path
    trajectory_file = arguments[0]

    # Check inputs exist
    ensure_path_exists(arguments)

    # If it exists, read it
    trajectory_lines = read_trajectory_txt_file(trajectory_file)
    not_fixed = True
    square_lines = [] if not_fixed else read_squares_txt_file("./out/simple_squares.txt")

    fig_path = trajectory_file.split('.txt')[0]

    if len(arguments) == 1:
        fig_path += ".png"
        # Plot it
        trajectory_tree_plot(trajectory_lines, fig_path, square_lines)

    if len(arguments) > 1:  # In this case, this means that we have to build a graph
        fig_path += "-graph.png"
        # Get graph files
        vrt_viz_files = get_vrt_viz_graph(arguments)

        # Read graph vertexes
        nodes = read_graph_vertices(vrt_viz_files[0])

        # Add successors
        add_graph_successors(vrt_viz_files[1], nodes)

        # Plot it
        trajectory_tree_plot(trajectory_lines, fig_path, square_lines, nodes, tree_depth=None)


if __name__ == '__main__':
    # Call to main running function
    main(arguments=sys.argv[1:])
