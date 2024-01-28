extends Node2D
class_name ShardEjection

@export var shardScene: PackedScene
var rng = RandomNumberGenerator.new()

func _ready():
	get_parent().get_node("GameManager").connect("game_over", eject_shards)


func eject_shards(message, didWin):
	if !didWin:
		for n in 100:
			var shard: RigidBody2D = shardScene.instantiate()
			add_child(shard)
			shard.apply_impulse(Vector2(rng.randf_range(-1.0, 1.0), rng.randf_range(-1.0, 1.0)).normalized() * 1000)
