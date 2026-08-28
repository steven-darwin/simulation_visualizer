import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import h5py
import matplotlib.animation as animation
import json
import sys

runtime_config_file_path = sys.argv[1]

with open(runtime_config_file_path) as runtime_config_file:
    runtime_config = json.load(runtime_config_file)

zone_file_path = (runtime_config['scmp']['staging_directory_path'] +
    "/" +
    runtime_config['scmp']['file_name_prefix'] +
    "_zone.json")

with open(zone_file_path) as zone_file:
    zone_data = json.load(zone_file)

fig = plt.figure(figsize = [15, 6])

artists = [];

if (runtime_config['mesh']['strategy'] == "transfinite_interpolation"):
    geometry_axis_field = ["x_coordinate", "y_coordinate", "z_coordinate"]
    axis_label = ["X", "Y", "Z"]

    for k in range(len(zone_data['zone'])):
        if "visualization" in zone_data['zone'][k] and zone_data['zone'][k]['type'] == "plane":
            for j in range(len(zone_data['zone'][k]['visualization'])):
                third_axis_min = third_axis_max = None
                first_axis_idx = second_axis_idx = None

                if zone_data['zone'][k]['coordinate'][0]['axis'] == "X":
                    first_axis_idx = 1
                    second_axis_idx = 2
                elif zone_data['zone'][k]['coordinate'][0]['axis'] == "Y":
                    first_axis_idx = 0
                    second_axis_idx = 2
                elif zone_data['zone'][k]['coordinate'][0]['axis'] == "Z":
                    first_axis_idx = 0
                    second_axis_idx = 1
            
                fig.suptitle(zone_data['zone'][k]['visualization'][j]['title'], fontsize=16)

                for i in range(len(zone_data['zone'][k]['visualization'][j]['source_file_suffix'])):
                    scatter_plot = fig.add_subplot(1, 2, 1, projection='3d')
                    heat_map = fig.add_subplot(1, 2, 2)

                    f = h5py.File(runtime_config['scmp']['staging_directory_path'] + 
                                  "/" + runtime_config['scmp']['file_name_prefix'] + 
                                  "." + zone_data['zone'][k]['name'] + ".item" + 
                                  str(i + 1) + '.h5', 'r')

                    entity_id_list = f['mesh']['geometry'].fields('vertex_id')[:]

                    first_axis = f['mesh']['geometry'].fields(geometry_axis_field[first_axis_idx])[:].tolist()
                    second_axis = f['mesh']['geometry'].fields(geometry_axis_field[second_axis_idx])[:].tolist()
                    third_axis = []

                    for entity_id in entity_id_list:
                        for parameter_iter in f['solver'][zone_data['zone'][k]['visualization'][j]['parameter']][:]:
                            if entity_id.tolist() == parameter_iter['entity_id'].tolist():
                                third_axis.append(float(parameter_iter['value']))
                                break

                    scatter_plot.scatter(np.array(first_axis), np.array(second_axis), np.array(third_axis))

                    scatter_plot.set_xlabel(axis_label[first_axis_idx])
                    scatter_plot.set_ylabel(axis_label[second_axis_idx])
                    scatter_plot.set_zlabel(zone_data['zone'][k]['visualization'][j]['parameter'])

                    if i == 0:
                        third_axis_min = min(third_axis)
                        third_axis_max = max(third_axis)

                    scatter_plot.set_zlim(third_axis_min, third_axis_max)

                    third_axis_im = np.empty([int(runtime_config['mesh']['transfinite_interpolation_segment_count']) + 1, int(runtime_config['mesh']['transfinite_interpolation_segment_count']) + 1])

                    for idx in range(len(third_axis)):
                        third_axis_im[round(int(runtime_config['mesh']['transfinite_interpolation_segment_count']) * (second_axis[idx] - np.array(second_axis).min()) / (np.array(second_axis).max() - np.array(second_axis).min()))][round(int(runtime_config['mesh']['transfinite_interpolation_segment_count']) * (first_axis[idx] - np.array(first_axis).min()) / (np.array(first_axis).max() - np.array(first_axis).min()))] = third_axis[idx]

                    heat_map_im = heat_map.imshow(
                        np.array(third_axis_im), 
                        extent = [
                            0, 
                            int(runtime_config['mesh']['transfinite_interpolation_segment_count']), 
                            0, 
                            int(runtime_config['mesh']['transfinite_interpolation_segment_count'])
                        ], 
                        interpolation='spline36', 
                        cmap="gray",
                        origin='lower', 
                        vmax=third_axis_max, 
                        vmin=third_axis_min
                    )

                    fig.colorbar(heat_map_im, ax = heat_map)
    
                    artists.append([scatter_plot, heat_map])
    
                    f.close()

                ani = animation.ArtistAnimation(fig = fig, artists = artists, interval = 200)
                ani.save(filename=(runtime_config['scmp']['staging_directory_path'] + 
                                   "/" + runtime_config['scmp']['file_name_prefix'] + 
                                   "." + zone_data['zone'][k]['name'] + 
                                   "." + zone_data['zone'][k]['visualization'][j]['parameter'] + 
                                   ".vszr.gif"), writer="pillow")

                plt.show()
