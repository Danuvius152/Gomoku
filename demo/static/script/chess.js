var chessBoard = [];
var color = 0; //黑0
var cnt = 0;
var isWin = false;
var robot_robot = false;
var not_init = true;
var playerSide = 0;
var tmp_playerSide = 0;
for (var i = 0; i < 15; i++) {
	chessBoard[i] = [];
	for (var j = 0; j < 15; j++) {
		chessBoard[i][j] = -1;
	}
}

var chess = document.getElementById('board');
var ctx = chess.getContext('2d');

ctx.strokeStyle = "#000";
var img = new Image();
img.src = "../static/image/board.png";

img.onload = function () {
	ctx.drawImage(img, 0, 0, 480, 480);
	drawChessBoard();
}

function drawChessBoard() {
	for (var i = 0; i < 15; i++) {
		ctx.moveTo(16 + i * 32, 16);  //起始点
		ctx.lineTo(16 + i * 32, 464); //终止点
		ctx.stroke();
		ctx.moveTo(16, 16 + i * 32);
		ctx.lineTo(464, 16 + i * 32);
		ctx.stroke();
	}
}
function choose_black() {
	tmp_playerSide = 1;
	document.getElementById("color").innerHTML = "当前Celty执黑子，点击开始游戏来挑战";
}

function choose_white() {
	tmp_playerSide = 0;
	document.getElementById("color").innerHTML = "当前Celty执白子，点击开始游戏来挑战";
}

function initChess() {
	not_init = false
	ctx.fillStyle = "#000000";
	ctx.beginPath();
	ctx.fillRect(0, 0, 480, 480);
	ctx.closePath();
	ctx.drawImage(img, 0, 0, 480, 480);
	drawChessBoard();
	
	playerSide = tmp_playerSide;
	for (var i = 0; i < 15; i++) {
		chessBoard[i] = [];
		for (var j = 0; j < 15; j++) {
			chessBoard[i][j] = -1;
		}
	}
	color = 0;
	cnt = 0;
	isWin = false;
	robot_robot = false

	$.ajax({
		url: "/initialize",
		type: "POST",
		data: String(playerSide),
		dataType: "text",
		success: function (data) {
			var tmp = data.split("@");
			alert(tmp[0]);
			if (tmp[1]) {
				var x = tmp[1].split(" ");
				var i = Number(x[0]);
				var j = Number(x[1]);
				oneStep(i, j, 0);
				chessBoard[i][j] = 0;
				++cnt;
				color ^= 1;
			}
		}
	})


	//if (playerSide == 1) {
	// $.ajax({
	// 	url: "/action",   
	// 	data: { row: String(-1), col: String(-1) },   
	// 	success: function (data) {  
	// 		var tmp = data.message.split(" ");
	// 		var i = Number(tmp[0]);
	// 		var j = Number(tmp[1]);
	// 		oneStep(i, j, 0);
	// 		chessBoard[i][j] = 0;
	// 		checkWin(i, j, color)
	// 		++cnt;
	// 		color ^= 1;
	// 	}
	// })
	// oneStep(2, 2, 0);
	// chessBoard[2][2] = 0;
	// ++cnt;
	// color ^= 1;
	//}
}


function oneStep(i, j, color) {
	ctx.beginPath();
	ctx.arc(16 + 32 * i, 16 + 32 * j, 13, 0, 2 * Math.PI)
	ctx.closePath();
	var gradient = ctx.createRadialGradient(16 + 32 * i + 2, 16 + 32 * j - 2, 13, 16 + 32 * i + 2, 16 + 32 * j - 2, 0);
	if (!color) {
		gradient.addColorStop(0, "#0b0b0b");
		gradient.addColorStop(1, "#636363");
	}
	else {
		gradient.addColorStop(0, "#d1d1d1");
		gradient.addColorStop(1, "#f8f8f8");
	}
	ctx.fillStyle = gradient;
	ctx.fill();
}


chess.onclick = function (e) {
	if (not_init) {
		alert("点击开始游戏唤醒Celty")
		return;
	}
	if (isWin)
		return;
	if (color == playerSide && !robot_robot) {
		var x = e.offsetX;
		var y = e.offsetY;
		var i = Math.floor(x / 32);
		var j = Math.floor(y / 32);
		if (chessBoard[i][j] == -1) {
			oneStep(i, j, color);
			if (!color)
				chessBoard[i][j] = 0;
			else
				chessBoard[i][j] = 1;

			checkWin(i, j, color)
			color ^= 1;
			++cnt;

			$.ajax({
				url: "/action",
				//type: "POST", 
				data: { row: String(i), col: String(j) },

				success: function (data) {
					var tmp = data.message.split(" ");
					var i = Number(tmp[0]);
					var j = Number(tmp[1]);
					if (i != -1) {
						oneStep(i, j, color);
						if (!color)
							chessBoard[i][j] = 0;
						else
							chessBoard[i][j] = 1;

						checkWin(i, j, color)
						color ^= 1;
						cnt++;
					}
					else {
						for (var i = 0; i < 15; i++) {
							for (var j = 0; j < 15; j++) {
								var val = chessBoard[i][j];
								if (val == -1) {
									continue;
								} else if (val == 1) {
									chessBoard[i][j] = 0;
									ctx.beginPath();
									ctx.arc(16 + 32 * i, 16 + 32 * j, 13, 0, 2 * Math.PI)
									ctx.closePath();
									var gradient = ctx.createRadialGradient(16 + 32 * i + 2, 16 + 32 * j - 2, 13, 16 + 32 * i + 2, 16 + 32 * j - 2, 0);
									gradient.addColorStop(0, "#0b0b0b");
									gradient.addColorStop(1, "#636363");
								} else if (val == 0) {
									chessBoard[i][j] = 1;
									ctx.beginPath();
									ctx.arc(16 + 32 * i, 16 + 32 * j, 13, 0, 2 * Math.PI)
									ctx.closePath();
									var gradient = ctx.createRadialGradient(16 + 32 * i + 2, 16 + 32 * j - 2, 13, 16 + 32 * i + 2, 16 + 32 * j - 2, 0);
									gradient.addColorStop(0, "#d1d1d1");
									gradient.addColorStop(1, "#f8f8f8");
								}
								ctx.fillStyle = gradient;
								ctx.fill();
							}
						}
						color ^= 1;
						window.setTimeout("alert('Celty flip the board!')", 100);
					}
				}
			})

		}
	}

	if (cnt == 225)
		window.setTimeout("alert('Draw!')", 100);
}

function checkWin(i, j, color) {
	var l = 0;
	var r = 0;
	while (j - l >= 0 && chessBoard[i][j - l] == chessBoard[i][j]) { ++l; }
	while (j + r <= 14 && chessBoard[i][j + r] == chessBoard[i][j]) { ++r; }

	if (l + r > 5) {
		if (!color) {
			window.setTimeout("alert('Black Wins!')", 100);
			isWin = true;
			return;
		} else {
			window.setTimeout("alert('White Wins!')", 100);
			isWin = true;
			return;
		}
	}

	l = 0; r = 0;
	while (i - l >= 0 && chessBoard[i - l][j] == chessBoard[i][j]) { ++l; }
	while (i + r < 15 && chessBoard[i + r][j] == chessBoard[i][j]) { ++r; }
	if (l + r > 5) {
		if (!color) {
			window.setTimeout("alert('Black Wins!')", 100);
			isWin = true;
			return;
		} else {
			window.setTimeout("alert('White Wins!')", 100);
			isWin = true;
			return;
		}
	}

	l = 0; r = 0;
	while (i - l >= 0 && j - l >= 0 && chessBoard[i - l][j - l] == chessBoard[i][j]) { ++l; }
	while (i + r < 15 && j + r < 15 && chessBoard[i + r][j + r] == chessBoard[i][j]) { ++r; }
	if (l + r > 5) {
		if (!color) {
			window.setTimeout("alert('Black Wins!')", 100);
			isWin = true;
			return;
		} else {
			window.setTimeout("alert('White Wins!')", 100);
			isWin = true;
			return;
		}
	}

	l = 0; r = 0;
	while (i - l >= 0 && j + l < 15 && chessBoard[i - l][j + l] == chessBoard[i][j]) { ++l; }
	while (i + r < 15 && j - r >= 0 && chessBoard[i + r][j - r] == chessBoard[i][j]) { ++r; }
	if (l + r > 5) {
		if (!color) {
			window.setTimeout("alert('Black Wins!')", 100);
			isWin = true;
			return;
		} else {
			window.setTimeout("alert('White Wins!')", 100);
			isWin = true;
			return;
		}

	}
}

function change() {
	if (cnt != 3 || !color)
		return;
	for (var i = 0; i < 15; i++) {
		for (var j = 0; j < 15; j++) {
			var val = chessBoard[i][j];
			if (val == -1) {
				continue;
			} else if (val == 1) {
				chessBoard[i][j] = 0;
				ctx.beginPath();
				ctx.arc(16 + 32 * i, 16 + 32 * j, 13, 0, 2 * Math.PI)
				ctx.closePath();
				var gradient = ctx.createRadialGradient(16 + 32 * i + 2, 16 + 32 * j - 2, 13, 16 + 32 * i + 2, 16 + 32 * j - 2, 0);
				gradient.addColorStop(0, "#0b0b0b");
				gradient.addColorStop(1, "#636363");
			} else if (val == 0) {
				chessBoard[i][j] = 1;
				ctx.beginPath();
				ctx.arc(16 + 32 * i, 16 + 32 * j, 13, 0, 2 * Math.PI)
				ctx.closePath();
				var gradient = ctx.createRadialGradient(16 + 32 * i + 2, 16 + 32 * j - 2, 13, 16 + 32 * i + 2, 16 + 32 * j - 2, 0);
				gradient.addColorStop(0, "#d1d1d1");
				gradient.addColorStop(1, "#f8f8f8");
			}
			ctx.fillStyle = gradient;
			ctx.fill();
		}
	}
	color ^= 1;

	$.ajax({
		url: "/action",
		//type: "POST", 
		data: { row: String(-1), col: String(-1) },

		success: function (data) {
			var tmp = data.message.split(" ");
			var i = Number(tmp[0]);
			var j = Number(tmp[1]);
			oneStep(i, j, 0);
			chessBoard[i][j] = 0;
			checkWin(i, j, color)
			++cnt;
			color ^= 1;
		}
	})
}

function init() {
	var step = new Array();
	$.ajax({
		url: "/start_jiji",
		async: false,
		type: "POST",
		data: String(playerSide),
		dataType: "text",
		success: (data) => {
			var tmp = data.split("@");
			alert(tmp[0]+" Please wait for a while");
			var x = tmp[1].split(" ");
			var i = Number(x[0]);
			var j = Number(x[1]);
			step[0] = i;
			step[1] = j;
			oneStep(i, j, 0);
			// sleep(500)

			chessBoard[i][j] = 0;
			++cnt;
			color ^= 1;
		}
	})
	return step;
}

function move_ai(i, j) {
	var step = new Array();
	$.ajax({
		url: "/action",
		//type: "POST",
		async: false,
		data: { row: String(i), col: String(j) },

		success: function (data) {
			var tmp = data.message.split(" ");
			var i = Number(tmp[0]);
			var j = Number(tmp[1]);
			step[0] = i;
			step[1] = j;
			if (i != -1) {
				oneStep(i, j, color);
				// sleep(500);
				if (!color)
					chessBoard[i][j] = 0;
				else
					chessBoard[i][j] = 1;

				checkWin(i, j, color)
				color ^= 1;
				cnt++;
			}
			else {
				for (var i = 0; i < 15; i++) {
					for (var j = 0; j < 15; j++) {
						var val = chessBoard[i][j];
						if (val == -1) {
							continue;
						} else if (val == 1) {
							chessBoard[i][j] = 0;
							ctx.beginPath();
							ctx.arc(16 + 32 * i, 16 + 32 * j, 13, 0, 2 * Math.PI)
							ctx.closePath();
							var gradient = ctx.createRadialGradient(16 + 32 * i + 2, 16 + 32 * j - 2, 13, 16 + 32 * i + 2, 16 + 32 * j - 2, 0);
							gradient.addColorStop(0, "#0b0b0b");
							gradient.addColorStop(1, "#636363");
						} else if (val == 0) {
							chessBoard[i][j] = 1;
							ctx.beginPath();
							ctx.arc(16 + 32 * i, 16 + 32 * j, 13, 0, 2 * Math.PI)
							ctx.closePath();
							var gradient = ctx.createRadialGradient(16 + 32 * i + 2, 16 + 32 * j - 2, 13, 16 + 32 * i + 2, 16 + 32 * j - 2, 0);
							gradient.addColorStop(0, "#d1d1d1");
							gradient.addColorStop(1, "#f8f8f8");
						}
						ctx.fillStyle = gradient;
						ctx.fill();
					}
				}
				color ^= 1;
			}
		}
	})
	return step;
}

function move_bs(i, j) {
	var step = new Array();
	$.ajax({
		url: "/action_bs",
		//type: "POST", 
		data: { row: String(i), col: String(j) },
		async: false,
		success: function (data) {
			var tmp = data.message.split(" ");
			var i = Number(tmp[0]);
			var j = Number(tmp[1]);
			step[0] = i;
			step[1] = j;
			if (i != -1) {
				oneStep(i, j, color);
				// sleep(500)
				if (!color)
					chessBoard[i][j] = 0;
				else
					chessBoard[i][j] = 1;

				checkWin(i, j, color)
				color ^= 1;
				cnt++;
			}
			else {
				for (var i = 0; i < 15; i++) {
					for (var j = 0; j < 15; j++) {
						var val = chessBoard[i][j];
						if (val == -1) {
							continue;
						} else if (val == 1) {
							chessBoard[i][j] = 0;
							ctx.beginPath();
							ctx.arc(16 + 32 * i, 16 + 32 * j, 13, 0, 2 * Math.PI)
							ctx.closePath();
							var gradient = ctx.createRadialGradient(16 + 32 * i + 2, 16 + 32 * j - 2, 13, 16 + 32 * i + 2, 16 + 32 * j - 2, 0);
							gradient.addColorStop(0, "#0b0b0b");
							gradient.addColorStop(1, "#636363");
						} else if (val == 0) {
							chessBoard[i][j] = 1;
							ctx.beginPath();
							ctx.arc(16 + 32 * i, 16 + 32 * j, 13, 0, 2 * Math.PI)
							ctx.closePath();
							var gradient = ctx.createRadialGradient(16 + 32 * i + 2, 16 + 32 * j - 2, 13, 16 + 32 * i + 2, 16 + 32 * j - 2, 0);
							gradient.addColorStop(0, "#d1d1d1");
							gradient.addColorStop(1, "#f8f8f8");
						}
						ctx.fillStyle = gradient;
						ctx.fill();
					}
				}
				color ^= 1;
			}
		}
	})
	return step;
}


function robotRobot() {
	robot_robot = true;
	ctx.fillStyle = "#000000";
	ctx.beginPath();
	ctx.fillRect(0, 0, 480, 480);
	ctx.closePath();
	ctx.drawImage(img, 0, 0, 480, 480);
	drawChessBoard();
	playerSide = tmp_playerSide;
	for (var i = 0; i < 15; i++) {
		chessBoard[i] = [];
		for (var j = 0; j < 15; j++) {
			chessBoard[i][j] = -1;
		}
	}
	color = 0;
	cnt = 0;
	isWin = false;

	// var x = new Array();
	// var y = new Array();
	// var n = 0
	// var m = 0
	var step = init()
	var i = step[0];
	var j = step[1];
	// x[++n] = i;
	// y[++m] = j;

	while (cnt <= 225) {
		if (isWin)
			break;
		if (color == 1 - playerSide) {
			var step = move_ai(i, j);
			var i = step[0];
			var j = step[1];
			// x[++n] = i;
			// y[++m] = j;
		}
		else {
			var step = move_bs(i, j);
			var i = step[0];
			var j = step[1];
			// x[++n] = i;
			// y[++m] = j;
		}
	}
	// sleep(500)
	// for (let i = 1; i <= n; ++i){
	// 	oneStep(x[i], y[i], (i + 1) % 2);
	// 	sleep(500)
	// }

}



function sleep(d) {
	for (var t = Date.now(); Date.now() - t <= d;);
}




