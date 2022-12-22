import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12

Item {
  objectName: "abc"
  property bool pBool: true
  property string pString: "ok"
  property int pInt: 42

  Item {
    objectName: "def"

    Item {
      objectName: "ghi"
    }
  }

  ListModel {
    id: fruitModel

    ListElement {
      name: "apple"
    }
    ListElement {
      name: "orange"
    }
    ListElement {
      name: "banana"
    }
  }

  Repeater {
    id: r
    model: fruitModel
    delegate: Item {
      objectName: name
    }
  }

  ListModel {
    id: vegetableModel

    ListElement {
      name: "artichoke"
    }
    ListElement {
      name: "cauliflower"
    }
    ListElement {
      name: "peas"
    }
  }

  ListView {
     objectName: "list"
     model: vegetableModel
     delegate: Item {
       objectName: name
     }
  }

  Item {
    objectName: "rangeA"
  }
  Item {
    objectName: "rangeA"
  }
  Item {
    objectName: "rangeB"
  }
  Item {
    objectName: "rangeB"
    Item {
      objectName: "foo"
    }
  }

  Item {
    objectName: "filters"
 
    Item {
      objectName: "filterA"
      property string p1: "A"
    }
    Item {
      objectName: "filterA"
      property string p1: "B"
    }
    Item {
      objectName: "filterA"
      property string p1: "B"
      Item {
       objectName: "filterB"
      }
    }
  }
}
