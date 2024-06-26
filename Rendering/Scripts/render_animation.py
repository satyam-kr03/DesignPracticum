import bpy
import os
import math

# Set the file paths
obj_file = "path/to/model.obj"
output_dir = "path/to/output/dir"
output_video = "render_animation.mp4"

# Clear existing objects and materials
bpy.ops.object.select_all(action='DESELECT')
bpy.ops.object.select_by_type(type='MESH')
bpy.ops.object.delete()

# Import the OBJ file
bpy.ops.wm.obj_import(filepath=obj_file)

# Get the imported object
imported_object = bpy.context.selected_objects[0]

# Set rendering engine to Cycles
bpy.context.scene.render.engine = 'CYCLES'

# Clear existing cameras
bpy.ops.object.select_all(action='DESELECT')
bpy.ops.object.select_by_type(type='CAMERA')
bpy.ops.object.delete()

# Add new camera
bpy.ops.object.camera_add()
camera = bpy.context.object
camera.location = (7.35889, -6.92579, 4.95831)
bpy.context.view_layer.objects.active = camera  # Set as active camera
bpy.context.scene.camera = camera  # Ensure it's recognized as the scene camera
bpy.context.scene.render.engine = 'BLENDER_WORKBENCH' # Set rendering engine to Blender Workbench
# Other options are CYCLES and BLENDER_EEVEE 

# Add a Track To constraint
bpy.ops.object.constraint_add(type='TRACK_TO')
constraint = camera.constraints['Track To']
constraint.target = imported_object
constraint.track_axis = 'TRACK_NEGATIVE_Z'
constraint.up_axis = 'UP_Y'

# Camera animation setup
radius = 200
center = imported_object.location
frames_per_second = 10
animation_length_seconds = 3
total_frames = frames_per_second * animation_length_seconds

bpy.context.scene.frame_start = 1
bpy.context.scene.frame_end = total_frames
bpy.context.scene.render.fps = frames_per_second

for frame in range(1, total_frames + 1):
    angle = 2 * math.pi * frame / total_frames
    x = center.x + radius * math.cos(angle)
    y = center.y + radius * math.sin(angle)
    camera.location = (x, y, 5)
    camera.keyframe_insert(data_path="location", frame=frame)

# Add lighting
bpy.ops.object.light_add(type='AREA', location=(5, 5, 10))
light = bpy.context.object
light.data.shape = 'RECTANGLE'
light.data.size = 5
light.data.size_y = 5
light.data.energy = 1000

# Setup rendering for video
bpy.context.scene.render.resolution_x = 1920
bpy.context.scene.render.resolution_y = 1080
bpy.context.scene.render.filepath = os.path.join(output_dir, output_video)
bpy.context.scene.render.image_settings.file_format = 'FFMPEG'
bpy.context.scene.render.ffmpeg.format = 'MPEG4'
bpy.context.scene.render.ffmpeg.codec = 'H264'

# Render the animation
bpy.ops.render.render(animation=True)

print("Rendering completed and saved as:", bpy.context.scene.render.filepath)