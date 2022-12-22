/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testqmlpath.h"

#include <QQmlApplicationEngine>
#include <QQuickItem>

#include "../src/qmlpath.h"

void TestQmlPath::parse_data() {
  QTest::addColumn<QString>("input");
  QTest::addColumn<bool>("result");

  // Testing the path parser
  QTest::addRow("empty") << "" << false;
  QTest::addRow("root") << "/" << true;
  QTest::addRow("search without key") << "//" << false;
  QTest::addRow("key") << "abc" << true;
  QTest::addRow("root+key") << "/abc" << true;
  QTest::addRow("search key") << "//abc" << true;
  QTest::addRow("two blocks") << "/abc/cba" << true;
  QTest::addRow("two blocks + search") << "/abc//cba" << true;
  QTest::addRow("two blocks + unfinished search") << "/abc/cba//" << false;
  QTest::addRow("two blocks + extra slash") << "/abc/cba/" << true;

  // Testing the array selection
  QTest::addRow("non-terminated array 1") << "/abc[" << false;
  QTest::addRow("non-terminated array 2") << "/abc[123" << false;
  QTest::addRow("root element + index") << "/[123]" << true;
  QTest::addRow("item + index") << "/abc[123]" << true;
  QTest::addRow("search + index") << "//abc[123]" << true;
  QTest::addRow("negative index") << "/abc[-1]" << false;
  QTest::addRow("complex ranges") << "/[123]/b[1]/c//d/e[0]/f/[6]" << true;

  // Property
  QTest::addRow("non-terminated property 1") << "/abc{" << false;
  QTest::addRow("non-terminated property 2") << "/abc{foo" << false;
  QTest::addRow("empty property 1") << "/abc{=}" << false;
  QTest::addRow("empty property 2") << "/abc{=b}" << false;
  QTest::addRow("terminated property without value") << "/abc{foo}" << true;
  QTest::addRow("terminated property with empty value") << "/abc{foo=}" << true;
  QTest::addRow("terminated property with value") << "/abc{foo=bar}" << true;
  QTest::addRow("item + property") << "/abc{foo}" << true;
  QTest::addRow("item + property + value") << "/abc{foo=bar}" << true;
  QTest::addRow("search + property") << "//abc{foo}" << true;
  QTest::addRow("search + property + value") << "//abc{foo=bar}" << true;

  // Property + array selection
  QTest::addRow("mix 1") << "/[123][1]" << true;
  QTest::addRow("mix 2") << "/{a}{b}" << true;
  QTest::addRow("mix 3") << "/{a}[1]{b}" << true;
  QTest::addRow("mix 4") << "/{a}[1]{b}" << true;
  QTest::addRow("mix 5") << "/{a}[1]{b}[2]" << true;
  QTest::addRow("mix 6") << "/[123]{a}[2]/b[1]{b=c}/c//d{d}/e[0]/f/[6]{e=f}[5]"
                         << true;
}

void TestQmlPath::parse() {
  QObject parent;
  QmlPath* qmlPath = new QmlPath(&parent);

  QFETCH(QString, input);
  QFETCH(bool, result);

  QCOMPARE(qmlPath->parse(input), result);
}

void TestQmlPath::evaluate_data() {
  QTest::addColumn<QString>("url");
  QTest::addColumn<QString>("input");
  QTest::addColumn<bool>("result");
  QTest::addColumn<QString>("name");

  QTest::addRow("select the root element") << "qrc:a.qml"
                                           << "/" << true << "abc";
  QTest::addRow("select the root element by name") << "qrc:a.qml"
                                                   << "/abc" << true << "abc";
  QTest::addRow("select an invalid root element by name")
      << "qrc:a.qml"
      << "/invalid" << false;
  QTest::addRow("select the nested element without name")
      << "qrc:a.qml"
      << "/abc/" << true << "def";
  QTest::addRow("select the nested element by name")
      << "qrc:a.qml"
      << "/abc/def" << true << "def";
  QTest::addRow("select the nested-nested element by name")
      << "qrc:a.qml"
      << "/abc/def/ghi" << true << "ghi";
  QTest::addRow("select an invalid nested element by name")
      << "qrc:a.qml"
      << "/abc/invalid" << false;
  QTest::addRow("select an invalid nested-nested element by name")
      << "qrc:a.qml"
      << "/abc/def/invalid" << false;
  QTest::addRow("search the root element") << "qrc:a.qml"
                                           << "//abc" << true << "abc";
  QTest::addRow("search an invalid element") << "qrc:a.qml"
                                             << "//invalid" << false;
  QTest::addRow("search the nested element") << "qrc:a.qml"
                                             << "//def" << true << "def";
  QTest::addRow("search the nested-nested element") << "qrc:a.qml"
                                                    << "//ghi" << true << "ghi";
  QTest::addRow("search the nested element, then the first element")
      << "qrc:a.qml"
      << "//def/" << true << "ghi";
  QTest::addRow("search the nested-nested element, plus the next item")
      << "qrc:a.qml"
      << "//ghi/" << false;
  QTest::addRow("search the nested element from an object")
      << "qrc:a.qml"
      << "/abc//def" << true << "def";
  QTest::addRow("search the nested-nested element from an object")
      << "qrc:a.qml"
      << "/abc//ghi" << true << "ghi";
  QTest::addRow("search an invalid element from an object")
      << "qrc:a.qml"
      << "/abc//invalid" << false;
  QTest::addRow("search from an object, then the first element")
      << "qrc:a.qml"
      << "/abc//def/" << true << "ghi";

  // Repeater crazyness: the objects are stored in the parent
  QTest::addRow("select for apple in repeater")
      << "qrc:a.qml"
      << "/abc/apple" << true << "apple";
  QTest::addRow("search for apple in repeater") << "qrc:a.qml"
                                                << "//apple" << true << "apple";

  // objects with contentItem property (lists)
  QTest::addRow("select item in lists")
      << "qrc:a.qml"
      << "/abc/list/artichoke" << true << "artichoke";
  QTest::addRow("search for item a lists")
      << "qrc:a.qml"
      << "//artichoke" << true << "artichoke";
  QTest::addRow("search for list, then item")
      << "qrc:a.qml"
      << "//list/artichoke" << true << "artichoke";

  // Indexing
  QTest::addRow("root index") << "qrc:a.qml"
                              << "/[0]" << true << "abc";
  QTest::addRow("root out of range") << "qrc:a.qml"
                                     << "/[1]" << false;
  QTest::addRow("select item in range") << "qrc:a.qml"
                                        << "/abc[0]" << true << "abc";
  QTest::addRow("select item out of range") << "qrc:a.qml"
                                            << "/abc[1]" << false;
  QTest::addRow("select item in range (0)")
      << "qrc:a.qml"
      << "/abc/rangeA[0]" << true << "rangeA";
  QTest::addRow("select item in range (1)")
      << "qrc:a.qml"
      << "/abc/rangeA[1]" << true << "rangeA";
  QTest::addRow("search for item in range") << "qrc:a.qml"
                                            << "//ghi[0]" << true << "ghi";
  QTest::addRow("search for item out of range") << "qrc:a.qml"
                                                << "/ghi[1]" << false;
  QTest::addRow("search for item in range (0)")
      << "qrc:a.qml"
      << "//rangeB[0]" << true << "rangeB";
  QTest::addRow("search for item in range (1)")
      << "qrc:a.qml"
      << "//rangeB[1]" << true << "rangeB";
  QTest::addRow("search for item in range, then something")
      << "qrc:a.qml"
      << "//rangeB[1]/foo" << true << "foo";

  // Property
  QTest::addRow("filter by property - no value")
      << "qrc:a.qml"
      << "/abc{pBool}" << true << "abc";
  QTest::addRow("filter by property - invalid") << "qrc:a.qml"
                                                << "/abc{p2}" << false;
  QTest::addRow("filter by property with value")
      << "qrc:a.qml"
      << "/abc{pBool=true}" << true << "abc";
  QTest::addRow("filter by property with value - not match")
      << "qrc:a.qml"
      << "/abc{pBool=false}" << false;
  QTest::addRow("filter by property (string)")
      << "qrc:a.qml"
      << "/abc{pString=ok}" << true << "abc";
  QTest::addRow("filter by property (int)") << "qrc:a.qml"
                                            << "/abc{pInt=42}" << true << "abc";

  // Property + index
  QTest::addRow("first index, then property")
      << "qrc:a.qml"
      << "//filterA[1]{p1=B}" << true << "filterA";
  QTest::addRow("first index, then property, plus extra indexing")
      << "qrc:a.qml"
      << "//filterA[1]{p1=B}[0]" << true << "filterA";
  QTest::addRow("first property, then index")
      << "qrc:a.qml"
      << "//filterA{p1=B}[0]" << true << "filterA";
  QTest::addRow("search by property")
      << "qrc:a.qml"
      << "//filters/{p1=B}[1]/filterB" << true << "filterB";
}

void TestQmlPath::evaluate() {
  QFETCH(QString, url);
  QQmlApplicationEngine engine(url);

  QmlPath* qmlPath = new QmlPath(&engine);

  QFETCH(QString, input);
  QVERIFY(qmlPath->parse(input));

  QFETCH(bool, result);
  QQuickItem* obj = qmlPath->evaluate(&engine);
  QCOMPARE(!!obj, result);

  if (obj) {
    QFETCH(QString, name);
    QCOMPARE(obj->objectName(), name);
  }
}

static TestQmlPath s_testQmlPath;
