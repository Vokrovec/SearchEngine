#include "common/HTMLParser.hpp"
#include "common/HTMLElement.hpp"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

class HTMLParserTest {
private:
    inline static const std::string TEST_FILE = "test_parser.html";

    static void writeFile(const std::string& html) {
        std::ofstream file(TEST_FILE);
        assert(file.is_open());

        file << html;
        file.close();
    }

    static void cleanup() {
        std::remove(TEST_FILE.c_str());
    }

public:

    static void testSimpleElement() {
        HTMLParser parser;

        bool called = false;

        parser.registerCallback("div", [&](const Element& element) {
            called = true;

            assert(element.getName() == "div");
            assert(element.getText() == "Hello");
            assert(element.getAttributes().empty());
        });

        writeFile("<div>Hello</div>");
        parser.parse(TEST_FILE);

        assert(called);

        cleanup();
    }

    static void testEmptyElement() {
        HTMLParser parser;

        bool called = false;

        parser.registerCallback("div", [&](const Element& element) {
            called = true;

            assert(element.getName() == "div");
            assert(element.getText().empty());
            assert(element.getAttributes().empty());
        });

        writeFile("<div></div>");
        parser.parse(TEST_FILE);

        assert(called);

        cleanup();
    }

    static void testMultipleElements() {
        HTMLParser parser;

        int calls = 0;

        parser.registerCallback("div", [&](const Element& element) {
            ++calls;

            assert(element.getName() == "div");
            assert(element.getAttributes().empty());
        });

        writeFile(
            "<div>one</div>"
            "<div>two</div>"
            "<div>three</div>"
        );

        parser.parse(TEST_FILE);

        assert(calls == 3);

        cleanup();
    }

    static void testAttributes() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            assert(element.getName() == "div");

            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 3);

            assert(attributes[0].Name == "id");
            assert(attributes[0].Value == "main");

            assert(attributes[1].Name == "class");
            assert(attributes[1].Value == "container");

            assert(attributes[2].Name == "data-value");
            assert(attributes[2].Value == "hello");
        });

        writeFile(
            R"(<div id="main" class="container" data-value="hello">Text</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testSingleQuotes() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 2);

            assert(attributes[0].Name == "id");
            assert(attributes[0].Value == "hello world");

            assert(attributes[1].Name == "class");
            assert(attributes[1].Value == "container");
        });

        writeFile(
            R"(<div id='hello world' class='container'>Text</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testBooleanAttributes() {
        HTMLParser parser;

        parser.registerCallback("input", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 3);

            assert(attributes[0].Name == "disabled");
            assert(attributes[0].Value.empty());

            assert(attributes[1].Name == "required");
            assert(attributes[1].Value.empty());

            assert(attributes[2].Name == "checked");
            assert(attributes[2].Value.empty());
        });

        writeFile(
            "<input disabled required checked>"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testAttributeWithSpaces() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 1);
            assert(attributes[0].Name == "class");
            assert(attributes[0].Value == "foo bar baz");
        });

        writeFile(
            R"(<div class="foo bar baz">Hello</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testAttributeWithEquals() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 1);
            assert(attributes[0].Name == "value");
            assert(attributes[0].Value == "a=b=c");
        });

        writeFile(
            R"(<div value="a=b=c">Hello</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testAttributeWithSpecialCharacters() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 2);

            assert(attributes[0].Name == "data-value");
            assert(attributes[0].Value == "hello/world?foo=bar&x=1");

            assert(attributes[1].Name == "title");
            assert(attributes[1].Value == "Hello, world!");
        });

        writeFile(
            R"(<div data-value="hello/world?foo=bar&x=1" title="Hello, world!">Text</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testColonInAttributeName() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 1);

            assert(attributes[0].Name == "x:test");
            assert(attributes[0].Value == "hello");
        });

        writeFile(
            R"(<div x:test="hello">Text</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testHyphenInAttributeName() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 2);

            assert(attributes[0].Name == "data-test");
            assert(attributes[0].Value == "123");

            assert(attributes[1].Name == "aria-label");
            assert(attributes[1].Value == "hello");
        });

        writeFile(
            R"(<div data-test="123" aria-label="hello">Text</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testNestedElements() {
        HTMLParser parser;

        bool divCalled = false;
        bool spanCalled = false;

        parser.registerCallback("div", [&](const Element& element) {
            divCalled = true;

            assert(element.getName() == "div");
        });

        parser.registerCallback("span", [&](const Element& element) {
            spanCalled = true;

            assert(element.getName() == "span");
        });

        writeFile(
            "<div>"
                "<span>Hello</span>"
            "</div>"
        );

        parser.parse(TEST_FILE);

        assert(divCalled);
        assert(spanCalled);

        cleanup();
    }

    static void testTextContainingSymbols() {
        HTMLParser parser;

        parser.registerCallback("p", [&](const Element& element) {
            assert(element.getName() == "p");
            assert(element.getText() == "Hello & goodbye!");
        });

        writeFile(
            "<p>Hello & goodbye!</p>"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testTextContainingQuotes() {
        HTMLParser parser;

        parser.registerCallback("p", [&](const Element& element) {
            assert(element.getText() == R"(He said "hello" and 'goodbye'.)");
        });

        writeFile(
            R"(<p>He said "hello" and 'goodbye'.</p>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testTextContainingAngleBrackets() {
        HTMLParser parser;

        parser.registerCallback("p", [&](const Element& element) {
            /*
             * This is intentionally weird because '<' has special
             * meaning to your parser.
             */
            assert(element.getName() == "p");
        });

        writeFile(
            "<p>1 &lt; 2 and 3 &gt; 2</p>"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testWhitespaceInText() {
        HTMLParser parser;

        parser.registerCallback("p", [&](const Element& element) {
            assert(!element.getText().empty());
        });

        writeFile(
            "<p>   Hello     world   </p>"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testNewlinesInText() {
        HTMLParser parser;

        parser.registerCallback("p", [&](const Element& element) {
            assert(!element.getText().empty());
        });

        writeFile(
            "<p>\n"
            "Hello\n"
            "World\n"
            "</p>"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testEmptyAttribute() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 1);
            assert(attributes[0].Name == "class");
            assert(attributes[0].Value.empty());
        });

        writeFile(
            R"(<div class="">Hello</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testUnquotedAttribute() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 1);
            assert(attributes[0].Name == "id");
            assert(attributes[0].Value == "hello");
        });

        writeFile(
            "<div id=hello>Hello</div>"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testAttributeWithoutValue() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 1);
            assert(attributes[0].Name == "disabled");
            assert(attributes[0].Value.empty());
        });

        writeFile(
            "<div disabled>Hello</div>"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testAttributeWhitespace() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 2);

            assert(attributes[0].Name == "id");
            assert(attributes[0].Value == "hello");

            assert(attributes[1].Name == "class");
            assert(attributes[1].Value == "world");
        });

        writeFile(
            R"(<div   id = "hello"    class = "world"   >Text</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testSelfClosing() {
        HTMLParser parser;

        bool called = false;

        parser.registerCallback("img", [&](const Element& element) {
            called = true;

            assert(element.getName() == "img");
        });

        writeFile(
            R"(<img src="image.png" />)"
        );

        parser.parse(TEST_FILE);

        assert(called);

        cleanup();
    }

    static void testMultipleSelfClosing() {
        HTMLParser parser;

        int calls = 0;

        parser.registerCallback("img", [&](const Element&) {
            ++calls;
        });

        writeFile(
            "<img/>"
            "<img />"
            "<img    />"
        );

        parser.parse(TEST_FILE);

        assert(calls == 3);

        cleanup();
    }

    static void testEmptyFile() {
        HTMLParser parser;

        bool called = false;

        parser.registerCallback("div", [&](const Element&) {
            called = true;
        });

        writeFile("");

        parser.parse(TEST_FILE);

        assert(!called);

        cleanup();
    }

    static void testOnlyWhitespace() {
        HTMLParser parser;

        bool called = false;

        parser.registerCallback("div", [&](const Element&) {
            called = true;
        });

        writeFile(
            "     \n"
            "\t\t"
            "     "
        );

        parser.parse(TEST_FILE);

        assert(!called);

        cleanup();
    }

    static void testOnlyText() {
        HTMLParser parser;

        bool called = false;

        parser.registerCallback("div", [&](const Element&) {
            called = true;
        });

        writeFile(
            "Hello world"
        );

        parser.parse(TEST_FILE);

        assert(!called);

        cleanup();
    }

    static void testUnknownElement() {
        HTMLParser parser;

        bool called = false;

        parser.registerCallback("div", [&](const Element&) {
            called = true;
        });

        writeFile(
            "<something>Hello</something>"
        );

        parser.parse(TEST_FILE);

        assert(!called);

        cleanup();
    }

    static void testUnknownElementAroundKnownElement() {
        HTMLParser parser;

        bool called = false;

        parser.registerCallback("div", [&](const Element&) {
            called = true;
        });

        writeFile(
            "<unknown>"
                "<div>Hello</div>"
            "</unknown>"
        );

        parser.parse(TEST_FILE);

        /*
         * This test is useful because it checks what your parser
         * does when an unknown tag contains a known tag.
         */
        assert(!called);

        cleanup();
    }

    static void testLargeText() {
        HTMLParser parser;

        std::string expected(10000, 'A');

        parser.registerCallback("p", [&](const Element& element) {
            assert(element.getText() == expected);
        });

        writeFile(
            "<p>" + expected + "</p>"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testLargeNumberOfAttributes() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 100);

            for (size_t i = 0; i < attributes.size(); ++i) {
                assert(
                    attributes[i].Name ==
                    "data-" + std::to_string(i)
                );

                assert(
                    attributes[i].Value ==
                    std::to_string(i)
                );
            }
        });

        std::string html = "<div ";

        for (int i = 0; i < 100; ++i) {
            html +=
                "data-" + std::to_string(i) +
                "=\"" + std::to_string(i) + "\" ";
        }

        html += ">Hello</div>";

        writeFile(html);

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testRepeatedAttributes() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 3);

            assert(attributes[0].Name == "class");
            assert(attributes[0].Value == "one");

            assert(attributes[1].Name == "class");
            assert(attributes[1].Value == "two");

            assert(attributes[2].Name == "class");
            assert(attributes[2].Value == "three");
        });

        writeFile(
            R"(<div class="one" class="two" class="three">Hello</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testAttributeValueContainingSlash() {
        HTMLParser parser;

        parser.registerCallback("a", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 1);
            assert(attributes[0].Value == "/foo/bar/baz");
        });

        writeFile(
            R"(<a href="/foo/bar/baz">Link</a>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testAttributeValueContainingGreaterThan() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 1);
            assert(attributes[0].Value == "a > b");
        });

        writeFile(
            R"(<div data-value="a > b">Text</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testAttributeValueContainingLessThan() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 1);
            assert(attributes[0].Value == "a < b");
        });

        /*
         * This is an important test because your parser searches
         * for '>' without understanding quoted strings.
         */
        writeFile(
            R"(<div data-value="a < b">Text</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testAttributeValueContainingGreaterThanAndEquals() {
        HTMLParser parser;

        parser.registerCallback("div", [&](const Element& element) {
            const auto& attributes = element.getAttributes();

            assert(attributes.size() == 1);
            assert(attributes[0].Value == "a >= b");
        });

        writeFile(
            R"(<div data-value="a >= b">Text</div>)"
        );

        parser.parse(TEST_FILE);

        cleanup();
    }

    static void testCallbackOrder() {
        HTMLParser parser;

        std::vector<std::string> calls;

        parser.registerCallback("div", [&](const Element&) {
            calls.push_back("div");
        });

        parser.registerCallback("span", [&](const Element&) {
            calls.push_back("span");
        });

        parser.registerCallback("p", [&](const Element&) {
            calls.push_back("p");
        });

        writeFile(
            "<div>"
                "<span>Hello</span>"
                "<p>World</p>"
            "</div>"
        );

        parser.parse(TEST_FILE);

        assert(calls.size() == 3);

        /*
         * These assertions are particularly useful for discovering
         * problems in recursive parsing.
         */
        assert(calls[0] == "span");
        assert(calls[1] == "p");
        assert(calls[2] == "div");

        cleanup();
    }

    static void testCaseSensitiveTag() {
        HTMLParser parser;

        bool lowercaseCalled = false;
        bool uppercaseCalled = false;

        parser.registerCallback("div", [&](const Element&) {
            lowercaseCalled = true;
        });

        parser.registerCallback("DIV", [&](const Element&) {
            uppercaseCalled = true;
        });

        writeFile("<DIV>Hello</DIV>");

        parser.parse(TEST_FILE);

        /*
         * Your current parser's tag set contains lowercase names,
         * so uppercase DIV should not be recognized.
         */
        assert(!lowercaseCalled);
        assert(!uppercaseCalled);

        cleanup();
    }

    static void testScriptElement() {
        HTMLParser parser;

        bool called = false;

        parser.registerCallback("script", [&](const Element& element) {
            called = true;

            assert(element.getName() == "script");
        });

        writeFile(
            R"(<script>if (a < b) { x > 10; }</script>)"
        );

        parser.parse(TEST_FILE);

        assert(called);

        cleanup();
    }

    static void testStyleElement() {
        HTMLParser parser;

        bool called = false;

        parser.registerCallback("style", [&](const Element& element) {
            called = true;

            assert(element.getName() == "style");
        });

        writeFile(
            R"(<style>body { width: 100%; }</style>)"
        );

        parser.parse(TEST_FILE);

        assert(called);

        cleanup();
    }

    static void testComment() {
        HTMLParser parser;

        int divCalls = 0;

        parser.registerCallback("div", [&](const Element&) {
            ++divCalls;
        });

        writeFile(
            "<!-- <div>Not really a div</div> -->"
            "<div>Real div</div>"
        );

        parser.parse(TEST_FILE);

        /*
         * This will probably fail with your current parser because
         * comments aren't supported.
         *
         * That's actually useful: it tells you that comments need
         * explicit handling if you want HTML-like behavior.
         */
        assert(divCalls == 1);

        cleanup();
    }

    static void testUnclosedElement() {
        HTMLParser parser;

        bool called = false;

        parser.registerCallback("div", [&](const Element& element) {
            called = true;
            assert(element.getName() == "div");
        });

        writeFile(
            "<div>Hello"
        );

        parser.parse(TEST_FILE);

        /*
         * Decide whether this should be accepted or rejected.
         * This assertion documents the intended behavior.
         */
        assert(called);

        cleanup();
    }

    static void testMismatchedClosingTag() {
        HTMLParser parser;

        bool called = false;

        parser.registerCallback("div", [&](const Element&) {
            called = true;
        });

        writeFile(
            "<div>Hello</span>"
        );

        parser.parse(TEST_FILE);

        /*
         * This is intentionally malformed.
         * Change the assertion according to the behavior you want.
         */
        assert(called);

        cleanup();
    }

    static void testMissingClosingTagWithSibling() {
        HTMLParser parser;

        int calls = 0;

        parser.registerCallback("div", [&](const Element&) {
            ++calls;
        });

        writeFile(
            "<div>First"
            "<div>Second</div>"
        );

        parser.parse(TEST_FILE);

        /*
         * This is an important malformed-input test.
         */
        assert(calls == 2);

        cleanup();
    }

    static void Run() {
        testSimpleElement();
        testEmptyElement();
        testMultipleElements();

        testAttributes();
        testSingleQuotes();
        testBooleanAttributes();
        testAttributeWithSpaces();
        testAttributeWithEquals();
        testAttributeWithSpecialCharacters();
        testColonInAttributeName();
        testHyphenInAttributeName();

        testNestedElements();

        testTextContainingSymbols();
        testTextContainingQuotes();
        testTextContainingAngleBrackets();
        testWhitespaceInText();
        testNewlinesInText();

        testEmptyAttribute();
        testUnquotedAttribute();
        testAttributeWithoutValue();
        testAttributeWhitespace();

        testSelfClosing();
        testMultipleSelfClosing();

        testEmptyFile();
        testOnlyWhitespace();
        testOnlyText();

        testUnknownElement();
        testUnknownElementAroundKnownElement();

        testLargeText();
        testLargeNumberOfAttributes();
        testRepeatedAttributes();

        testAttributeValueContainingSlash();
        testAttributeValueContainingGreaterThan();
        testAttributeValueContainingLessThan();
        testAttributeValueContainingGreaterThanAndEquals();

        testCallbackOrder();

        testCaseSensitiveTag();

        testScriptElement();
        testStyleElement();

        testComment();

        testUnclosedElement();
        testMismatchedClosingTag();
        testMissingClosingTagWithSibling();
    }
};
