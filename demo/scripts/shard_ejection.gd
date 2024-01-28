extends Sprite2D
class_name ShardEjection

@export var shardScene: PackedScene
var rng = RandomNumberGenerator.new()

func _ready():
	var manager = get_parent().get_node("GameManager")
	manager.connect("game_over", game_over)
	manager.connect("block_shattered", block_shattered)
	manager.connect("jar_collision", jar_collision)
	manager.connect("jar_moves_up", jar_moves_up)


func game_over(message, didWin):
	if !didWin:
		for n in 100:
			var shard: RigidBody2D = shardScene.instantiate()
			add_child(shard)
			shard.apply_impulse(Vector2(rng.randf_range(-1.0, 1.0), rng.randf_range(-1.0, 1.0)).normalized() * 1000)
		texture = null
		get_node("AudioShatteredJar").play()

func block_shattered():
	get_node("AudioShatteredBlock").play()
	
func jar_collision(is_heavy):
	if is_heavy:
		get_node("AudioHeavyCollision").play()
	else:
		get_node("AudioLightCollision").play()
	
func jar_moves_up():
	get_node("AudioJarMovesUp").play()
