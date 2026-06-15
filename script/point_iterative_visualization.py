import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import h5py
import matplotlib.animation as animation

fig = plt.figure(figsize = [15, 9])

artists = [];

for i in range(11):
    top_middle_ax = fig.add_subplot(2, 3, 1, projection='3d')
    center_middle_ax = fig.add_subplot(2, 3, 2, projection='3d')
    bottom_middle_ax = fig.add_subplot(2, 3, 3, projection='3d')
    top_middle_ax2 = fig.add_subplot(2, 3, 4)
    center_middle_ax2 = fig.add_subplot(2, 3, 5)
    bottom_middle_ax2 = fig.add_subplot(2, 3, 6)

    if i == 0:
        f = h5py.File('../../../build/staging/slab_pre_solver.h5', 'r')
    else:
        f = h5py.File('../../../build/staging/slab_solver_iter' + str(i - 1) + '.h5', 'r')

    top_middle_plane_idx = []
    center_middle_plane_idx = []
    bottom_middle_plane_idx = []

    for idx, vertex in enumerate(f['mesh']['geometry'][:, 2].tolist()):
        if abs(vertex) < 0.001:
            center_middle_plane_idx.append(idx)
        elif abs(vertex - 25) < 0.001:
            top_middle_plane_idx.append(idx)
        elif abs(vertex + 25) < 0.001:
            bottom_middle_plane_idx.append(idx)

    X1 = []
    Y1 = []
    Z1 = []

    for idx in top_middle_plane_idx:
        X1.append(f['mesh']['geometry'][idx, 0])
        Y1.append(f['mesh']['geometry'][idx, 1])
        Z1.append(f['solver']['temperature'][idx, 0])

    top_middle_ax.scatter(np.array(X1), np.array(Y1), np.array(Z1))

    top_middle_ax.set_xlabel('X')
    top_middle_ax.set_ylabel('Y')
    top_middle_ax.set_zlabel('Temperature')
    top_middle_ax.set_title('Top Middle - Z Plane')
    top_middle_ax.set_zlim(20, 100)

    Z1_im = np.empty([5, 5])

    for idx in top_middle_plane_idx:
        Z1_im[round(4 * (f['mesh']['geometry'][idx, 1] - np.array(Y1).min()) / (np.array(Y1).max() - np.array(Y1).min()))][round(4 * (f['mesh']['geometry'][idx, 0] - np.array(X1).min()) / (np.array(X1).max() - np.array(X1).min()))] = f['solver']['temperature'][idx, 0]

    top_middle_ax_im = top_middle_ax2.imshow(np.array(Z1_im), 
                                                   extent = [0, 4, 0, 4], interpolation='spline36', cmap="gray",
                                                   origin='lower', vmax=100, vmin=20)
    fig.colorbar(top_middle_ax_im, ax = top_middle_ax2)

    X2 = []
    Y2 = []
    Z2 = []

    for idx in center_middle_plane_idx:
        X2.append(f['mesh']['geometry'][idx, 0])
        Y2.append(f['mesh']['geometry'][idx, 1])
        Z2.append(f['solver']['temperature'][idx, 0])

    center_middle_ax.scatter(np.array(X2), np.array(Y2), np.array(Z2))

    center_middle_ax.set_xlabel('X')
    center_middle_ax.set_ylabel('Y')
    center_middle_ax.set_zlabel('Temperature')
    center_middle_ax.set_title('Center Middle - Z Plane')
    center_middle_ax.set_zlim(20, 100)

    Z2_im = np.empty([5, 5])

    for idx in center_middle_plane_idx:
        Z2_im[round(4 * (f['mesh']['geometry'][idx, 1] - np.array(Y2).min()) / (np.array(Y2).max() - np.array(Y2).min()))][round(4 * (f['mesh']['geometry'][idx, 0] - np.array(X2).min()) / (np.array(X2).max() - np.array(X2).min()))] = f['solver']['temperature'][idx, 0]

    center_middle_ax_im = center_middle_ax2.imshow(np.array(Z2_im), 
                                                   extent = [0, 4, 0, 4], interpolation='spline36', cmap="gray",
                                                   origin='lower', vmax=100, vmin=20)
    fig.colorbar(center_middle_ax_im, ax = center_middle_ax2)

    X3 = []
    Y3 = []
    Z3 = []

    for idx in bottom_middle_plane_idx:
        X3.append(f['mesh']['geometry'][idx, 0])
        Y3.append(f['mesh']['geometry'][idx, 1])
        Z3.append(f['solver']['temperature'][idx, 0])

    bottom_middle_ax.scatter(np.array(X3), np.array(Y3), np.array(Z3))

    bottom_middle_ax.set_xlabel('X')
    bottom_middle_ax.set_ylabel('Y')
    bottom_middle_ax.set_zlabel('Temperature')
    bottom_middle_ax.set_title('Bottom Middle - Z Plane')
    bottom_middle_ax.set_zlim(20, 100)

    Z3_im = np.empty([5, 5])

    for idx in bottom_middle_plane_idx:
        Z3_im[round(4 * (f['mesh']['geometry'][idx, 1] - np.array(Y3).min()) / (np.array(Y3).max() - np.array(Y3).min()))][round(4 * (f['mesh']['geometry'][idx, 0] - np.array(X3).min()) / (np.array(X3).max() - np.array(X3).min()))] = f['solver']['temperature'][idx, 0]

    bottom_middle_ax_im = bottom_middle_ax2.imshow(np.array(Z3_im), 
                                                   extent = [0, 4, 0, 4], interpolation='spline36', cmap="gray",
                                                   origin='lower', vmax=100, vmin=20)
    fig.colorbar(bottom_middle_ax_im, ax = bottom_middle_ax2)

    artists.append([top_middle_ax, center_middle_ax, bottom_middle_ax, top_middle_ax2, center_middle_ax2, bottom_middle_ax2])

    f.close()

ani = animation.ArtistAnimation(fig = fig, artists = artists, interval = 250)
ani.save(filename="../../../build/staging/slab_visualization.gif", writer="pillow")

plt.show()
