import bpy
import os

# Set the file paths
obj_file = "path/to/model.obj"
output_dir = "path/to/output/dir"
output_image = "render.png"

# Set the device_type
bpy.context.preferences.addons[
    "cycles"
].preferences.compute_device_type = "CUDA"

# Set the device and feature set
bpy.context.scene.cycles.device = "GPU"

# get_devices() to let Blender detects GPU device
bpy.context.preferences.addons["cycles"].preferences.get_devices()
print(bpy.context.preferences.addons["cycles"].preferences.compute_device_type, "detected")

for device in bpy.context.preferences.addons["cycles"].preferences.devices:
    device["use"] = 1 # Using all devices, include GPU and CPU
    print("Using: ", device["name"])

# Clear existing objects and materials
bpy.ops.object.select_all(action='DESELECT')
bpy.ops.object.select_by_type(type='MESH')
bpy.ops.object.delete(use_global=False)

# Import OBJ file with materials
bpy.ops.wm.obj_import(filepath=obj_file)

# Get the imported objects
imported_objects = bpy.context.selected_objects

# Set Cycles as the rendering engine
bpy.context.scene.render.engine = 'CYCLES'

# Camera setup
# Delete existing cameras first
bpy.ops.object.select_all(action='DESELECT')
bpy.ops.object.select_by_type(type='CAMERA')
bpy.ops.object.delete(use_global=False)

# Add a new camera
bpy.ops.object.camera_add(location=(7.35889, -6.92579, 4.95831), rotation=(63.5593, 0, 46.6919))
camera = bpy.context.object

# Point the camera to the object
bpy.ops.object.select_all(action='DESELECT')
camera.select_set(True)
imported_objects[0].select_set(True)  # Select the first imported object
bpy.context.view_layer.objects.active = imported_objects[0]
bpy.ops.object.constraint_add(type='TRACK_TO')
bpy.context.object.constraints["Track To"].target = imported_objects[0]
bpy.context.object.constraints["Track To"].track_axis = 'TRACK_NEGATIVE_Z'
bpy.context.object.constraints["Track To"].up_axis = 'UP_Y'

# Adjust the camera to ensure the object is fully visible
bpy.context.scene.camera = camera
bpy.ops.view3d.camera_to_view_selected()

# Add an area light
bpy.ops.object.light_add(type='AREA', location=(5, 5, 10))
light = bpy.context.object
light.data.shape = 'RECTANGLE'
light.data.size = 5
light.data.size_y = 5
light.data.energy = 200  # Adjust the light energy as needed

# Improve world lighting for a soft ambient effect
bpy.context.scene.world.use_nodes = True
world = bpy.context.scene.world
bg = world.node_tree.nodes['Background']
bg.inputs[0].default_value = (1, 1, 1, 1)  # White color
bg.inputs[1].default_value = 1.0  # Strength of the world background light

# Set render output path
bpy.context.scene.render.filepath = os.path.join(output_dir, output_image)

# Render the scene
bpy.ops.render.render(write_still=True)

print("Done")
print("\nRendering completed and saved as:", bpy.context.scene.render.filepath)