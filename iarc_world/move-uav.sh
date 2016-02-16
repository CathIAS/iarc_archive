
msg='{model_state: { model_name: iarc_uav, pose: { position: '
msg="${msg}{ x: $1, y: $2 ,z: $3 }"
msg="${msg}, orientation: {x: 0, y: 0, z: 0, w: 1 } }, twist: { linear: {x: 0.0 , y: 0 ,z: 0 } , angular: { x: 0.0 , y: 0 , z: 0.0 } } , reference_frame: world } }"

rosservice call /gazebo/set_model_state "$msg"
