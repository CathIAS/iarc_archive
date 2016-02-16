msg='{model_name: iarc_uav, pose: { position: { x: '
msg="${msg}$1, y: $2, z: $3 },"
msg="${msg} orientation: {x: 0, y: 0, z: 0, w: 1 } }, twist: { linear: { x: 0, y: 0, z: 0 }, angular: { x: 0, y: 0, z: 0}  }, reference_frame: world }" 
rostopic pub -r 60 /gazebo/set_model_state gazebo_msgs/ModelState "$msg"
