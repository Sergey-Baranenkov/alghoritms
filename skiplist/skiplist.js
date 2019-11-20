class Node {
  constructor(value) {
    this.value = value;
  }

  insertAfter(node) {
    //установить после определенного node
    this.right = node.right; //указателем на правый элемент вставляемого нода делаем указатель на правый элемент до вставляемого
    node.right && (node.right.left = this); //если элемент до вставляемого не последний, то ставим следующему элементу после вставляемого указатель на этот нод
    node.right = this; //предыдущему элементу ставим ссыку на этот элемент
    this.left = node; //этому элементу ставим ссылку на предыдущий
  }

  insertOn(node) {
    //устанавливает уровни
    this.up = node.up; //ставим указатель на верхний элемент вставляемого нода на верхний элемент до вставляемого
    node.up && (node.up.down = this); //если элемент до вставляемого не последний, то ставим следующему элементу после вставляемого указатель на этот
    node.up = this; //предыдущему элементу ставим ссылку на этот
    this.down = node; //этому на предыдущий
  }

  remove() {
    //удаляет этот элемент и связывает оставшиеся
    this.down && (this.down.up = this.up);
    this.left && (this.left.right = this.right);
    this.right && (this.right.left = this.left);
    this.up && (this.up.down = this.up);
  }
}

class Skiplist {
  constructor() {
	this.levels = 1;
    this.head = new Node(-Infinity);
	this.elements = 1
  }

  insert(value) {
    const drops = []; //массив для запоминания пройденых вниз уровней
    return function insert(at, value) {
      let level = 1;
      if (at.value === value) return false; //если уже есть такое значение - ничего не делаем

      if (at.right && at.right.value <= value) {
        //если значение справа меньше - идем вправо
        return insert.call(this, at.right, value); //call чтобы не терять контекст this
      }

      if (at.down) {
        //если есть значение снизу - идем вниз
        drops.push(at); //сохраняем указатели на элементы, после которых, возможно, поставим новый нод
        level++;
        return insert.call(this, at.down, value); //call чтобы не терять контекст this
      }

      //нашли место куда вставить
      let newNode = new Node(value); //вставляем новый нод
      newNode.insertAfter(at); //пушим элемент на самый нижний уровень
      while (this.levels < Math.log(this.elements) && coinFlip()) {
        //рандомно выбираем пушить ли элемент выше чем на самом низком уровне
        const promote = new Node(value);
        promote.insertOn(newNode); //пушим
        newNode = promote;

        const drop = drops.pop(); //берем элементы уровней до, после которых нужно вставить элемент
        if (drop) {
          //если есть такой элемент
          promote.insertAfter(drop); //вставляем после него
        } else {
          //иначе инициализируем новый уровень
          const newHead = new Node(-Infinity);
          newHead.insertOn(this.head); //добавляем его на уровень выше начала текущего верхнего уровня
          this.head = newHead; //верхний уровень теперь добавленый
          this.levels++; //увеличиваем счетчик уровней
          promote.insertAfter(this.head); //
        }
        level++;
      }
      //конец
      this.elements++; //увеличиваем счетчик элементов
      return true;
    }.call(this, this.head, value);
  }

  remove(value) {
    return function remove(at, value) {
      if (at.value === value) {
        //если нашли такое значение
        this.elements--;
        while (at) {
          at.remove();
          if (at.left === this.head && !at.right && this.head.down) {
            //если уровень пустой
            this.head = this.head.down; //удаляем весь уровень
            this.head.up = null; // удаляем указатель
            this.levels--; //на 1 уровень стало меньше
          }
          at = at.down; //сдвигаемся вниз
        }
        return true; //удалили
      }

      //иначе пробуем идти вправо
      if (at.right && at.right.value <= value) {
        return remove.call(this, at.right, value); //call чтобы не терять контекст this
      }

      // иначе идем вниз
      if (at.down) {
        return remove.call(this, at.down, value); //call чтобы не терять контекст this
      }
      //не нашлось объекта для удаления
      return false;
    }.call(this, this.head, value);
  }

  search(value) {
    return (function search(at, value) {
      if (at.value === value) return true; //если нашли такое число

      if (at.right && at.right.value <= value) {
        //иначе идем вправо
        return search(at.right, value);
      }

      if (at.down) {
        //иначе идем вниз
        return search(at.down, value);
      }

      //не нашли
      return false;
    })(this.head, value);
  }
  // превратить лист в двумерный массив  (для отображения)
  report() {
    const nodes = [];
    let level = 0;
    let head = this.head;
    let at = this.head;
    while (at) {
      //поуровнево заполняем
      nodes[level] = nodes[level] || [];
      nodes[level].push(at.value);
      if (at.right) {
        at = at.right;
      } else {
        level++;
        at = head.down;
        head = head.down;
      }
    }
    const basenode = nodes[nodes.length - 1];
    for (let i = 0; i < nodes.length - 1; i++) {
      const line = Array(basenode.length).fill(null); //null значит пропустить значение.
      let h = 0;
      for (let j = 0; j < basenode.length; j++) {
        if (nodes[i][h] == basenode[j]) {
          line[j] = basenode[j];
          h++;
        }
      }
      nodes[i] = line;
    }
    return nodes;
  }
}

const coinFlip = () => Math.random() < 0.5;
