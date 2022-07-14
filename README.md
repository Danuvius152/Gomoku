## 程序设计与数据结构III 五子棋项目
负责人：陈文迪 游灏溢

Codebase Credit: [张志成](https://github.com/Gabr1e1)

### 课程内容

- 基于树搜索的棋类AI设计基本思想与相关算法
- 基于强化学习和自我博弈的现代AI算法（以Alpha Zero为例）
- Web前端开发技术

### 项目要求

- **AI单步推理时间限制：5s**
- 必做：实现Minimax搜索，alpha-beta剪枝，迭代加深搜索，性能优化（例如启发式搜索，Zobrist缓存），支持人机对战的前端
- 选做：蒙特卡洛树搜索，强化学习相关算法的探索，或是其他创新性的优化策略
- 60%算法实现(70%：超过50%胜率打败Baseline，30%：Code Review)
- 40%前端展示

### 开发指南
#### 环境
- 请使用WSL或MacOS开发
#### API
请参照`judge/sample.cpp`的格式实现你的AI，其中：
- `init`: 初始化函数，会在程序开始的时候被调用

- `action(loc)`: `loc`参数为对手采取的行动，需要返回你在这步落子的位置，**注意：** 如果是第3步，返回`(-1,-1)`表示交换颜色，返回坐标表示不选择换手，正常下棋

- `ai_side`变量表示你的AI是哪一边，0黑1白，`ai_name`可以随便填写

 `AIController.h`不需要修改，提交时也仅需要提交`yourcode.cpp`

#### 算法评测

- 安装以下python包
  ```
  subprocess
  timeout_decorator
  numpy
  sys
  time
  ```

- 运行方式：编译你的AI，接着执行`python judge.py ai0path/human ai1path/human`，写在前面的表示先手，后面的表示后手，例如：
python3 judge.py ./sample human
python3 judge.py ./baseline human
python3 judge.py ./sample ./baseline 
python3 judge.py ./baseline ./sample
python3 judge.py ./baseline ./baseline

- **注意：** 你可以通过更改judge.py来输出更多信息

### 可供学习的参考资料

#### 算法
- 基础教程：https://github.com/lihongxun945/myblog/issues/11
- MCTS：https://www.geeksforgeeks.org/ml-monte-carlo-tree-search-mcts/
- AlphaZero论文：https://www.science.org/doi/10.1126/science.aar6404

#### 前端
- W3Schools https://www.w3school.com.cn/

