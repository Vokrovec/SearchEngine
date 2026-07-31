#include "common/HTMLElement.hpp"
#include "makros.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>

class HTMLElementTest {
  public:
      static void Run() {
          std::cout << "Testing HTMLElement class: " << std::endl;
          test_getTagName();
          test_ElementConstructor();
          std::cout << "All tests passed in HTMLElement class. " << std::endl;
      }
     
  private:
      static void test_getTagName() {
          // Basic tags
          assert(Element::getTagName("<div>") == "div");
          assert(Element::getTagName("<html>") == "html");
          assert(Element::getTagName("<body>") == "body");
          assert(Element::getTagName("<p>") == "p");

          // Tags with attributes
          assert(Element::getTagName("<div class=\"test\">") == "div");
          assert(Element::getTagName("<input type=\"text\">") == "input");
          assert(Element::getTagName("<a href=\"https://example.com\">") == "a");

          // Multiple attributes
          assert(Element::getTagName("<div id=\"main\" class=\"container\">") == "div");

          // Closing tags
          assert(Element::getTagName("</div>") == "/div");
          assert(Element::getTagName("</html>") == "/html");

          // Whitespace after tag name
          assert(Element::getTagName("<div class=\"test\">") == "div");
          assert(Element::getTagName("<div\tclass=\"test\">") == "div");
          assert(Element::getTagName("<div\nclass=\"test\">") == "div");

          // No attributes
          assert(Element::getTagName("<span>") == "span");

          // Single-character tag
          assert(Element::getTagName("<p>") == "p");

          // Empty input
          TEST_EXCEPTION(Element::getTagName(""), std::runtime_error);

          // Too short to contain a tag name
          TEST_EXCEPTION(Element::getTagName("<"), std::runtime_error);

          // Only whitespace
          TEST_EXCEPTION(Element::getTagName(" "), std::runtime_error);

          // Opening bracket followed by whitespace
          TEST_EXCEPTION(Element::getTagName("< "), std::runtime_error);

          // Newline after opening bracket
          TEST_EXCEPTION(Element::getTagName("<\n"), std::runtime_error);

          // Tab after opening bracket
          TEST_EXCEPTION(Element::getTagName("<\t"), std::runtime_error);

          // Missing '<'
          TEST_EXCEPTION(Element::getTagName("div>"), std::runtime_error);

          // Garbage input
          TEST_EXCEPTION(Element::getTagName("hello"), std::runtime_error);

          // Just a closing bracket
          TEST_EXCEPTION(Element::getTagName(">"), std::runtime_error);

          // Whitespace before tag
          TEST_EXCEPTION(Element::getTagName(" <div>"), std::runtime_error);
      }
      static void test_ElementConstructor() {
          Element e1("<div>");
          assert(e1.m_Name == "div");
          assert(e1.m_Attributes.empty());

          Element e2("<html>");
          assert(e2.m_Name == "html");
          assert(e2.m_Attributes.empty());

          Element e3("<body>");
          assert(e3.m_Name == "body");
          assert(e3.m_Attributes.empty());

          Element e4("<div class=\"container\">");

          assert(e4.m_Name == "div");
          assert(e4.m_Attributes.size() == 1);

          assert(e4.m_Attributes[0].Name == "class");
          assert(e4.m_Attributes[0].Value == "container");

          TEST_EXCEPTION(Element("<div"), std::runtime_error);

          TEST_EXCEPTION(
              Element("<div class=\"test\""),
              std::runtime_error
          );

          TEST_EXCEPTION(
              Element("<input type=\"text\""),
              std::runtime_error
          );

          TEST_EXCEPTION(
              Element("<div class=\"hello>"),
              std::runtime_error
          );

          TEST_EXCEPTION(
              Element("<a href=\"https://example.com>"),
              std::runtime_error
          );

          Element e(
              "<div id=\"main\" class=\"container\" data-id=\"12345\" title=\"Hello World\">"
          );

          assert(e.m_Name == "div");
          assert(e.m_Attributes.size() == 4);

          assert(e.m_Attributes[0].Name == "id");
          assert(e.m_Attributes[0].Value == "main");

          assert(e.m_Attributes[1].Name == "class");
          assert(e.m_Attributes[1].Value == "container");

          assert(e.m_Attributes[2].Name == "data-id");
          assert(e.m_Attributes[2].Value == "12345");

          assert(e.m_Attributes[3].Name == "title");
          assert(e.m_Attributes[3].Value == "Hello World");

          Element e5("<a href=\"https://example.com/test?a=1&b=2\">");

          assert(e5.m_Attributes.size() == 1);
          assert(e5.m_Attributes[0].Name == "href");
          assert(e5.m_Attributes[0].Value == "https://example.com/test?a=1&b=2");
          Element e6(
              "<img src=\"https://example.com/image.jpg\" "
              "alt=\"A beautiful image\">"
          );

          assert(e6.m_Attributes.size() == 2);

          assert(e6.m_Attributes[0].Name == "src");
          assert(e6.m_Attributes[0].Value == "https://example.com/image.jpg");

          assert(e6.m_Attributes[1].Name == "alt");
          assert(e6.m_Attributes[1].Value == "A beautiful image");

          Element e7("<input value=\"\">");

          assert(e7.m_Attributes.size() == 1);
          assert(e7.m_Attributes[0].Name == "value");
          assert(e7.m_Attributes[0].Value == "");

          Element e8(
              "<div id=\"main\" class=\"container\" "
              "title=\"Hello World\" data-id=\"123\" "
              "role=\"button\" tabindex=\"0\">"
          );

          assert(e8.m_Name == "div");
          assert(e8.m_Attributes.size() == 6);

          assert(e8.m_Attributes[0].Name == "id");
          assert(e8.m_Attributes[0].Value == "main");

          assert(e8.m_Attributes[1].Name == "class");
          assert(e8.m_Attributes[1].Value == "container");

          assert(e8.m_Attributes[2].Name == "title");
          assert(e8.m_Attributes[2].Value == "Hello World");

          assert(e8.m_Attributes[3].Name == "data-id");
          assert(e8.m_Attributes[3].Value == "123");

          assert(e8.m_Attributes[4].Name == "role");
          assert(e8.m_Attributes[4].Value == "button");

          assert(e8.m_Attributes[5].Name == "tabindex");
          assert(e8.m_Attributes[5].Value == "0");

          Element E1("<div id=\"test\">");
          Element E2("<div   id=\"test\">");
          Element E3("<div    id=\"test\"    >");
          Element E4("<div\tid=\"test\">");

          TEST_EXCEPTION(Element(""), std::runtime_error);
          TEST_EXCEPTION(Element("div"), std::runtime_error);
          TEST_EXCEPTION(Element("div>"), std::runtime_error);
          TEST_EXCEPTION(Element("hello world"), std::runtime_error);

          TEST_EXCEPTION(Element("<"), std::runtime_error);
          TEST_EXCEPTION(Element("< "), std::runtime_error);
          TEST_EXCEPTION(Element("<>"), std::runtime_error);

          TEST_EXCEPTION(
              Element("<div class=\"hello>"),
              std::runtime_error
          );

          TEST_EXCEPTION(
              Element("<a href=\"https://example.com>"),
              std::runtime_error
          );

          TEST_EXCEPTION(
              Element("<input value=\"test>"),
              std::runtime_error
          );
          TEST_EXCEPTION(
              Element("<div id=>"),
              std::runtime_error
          );

          TEST_EXCEPTION(
              Element("<div id= >"),
              std::runtime_error
          );
          TEST_EXCEPTION(
              Element("<div class>"),
              std::runtime_error
          );

          TEST_EXCEPTION(
              Element("<div id>"),
              std::runtime_error
          );

          TEST_EXCEPTION(
              Element("<div class \"test\">"),
              std::runtime_error
          );
          TEST_EXCEPTION(
              Element("<div id=test>"),
              std::runtime_error
          );

          TEST_EXCEPTION(
              Element("<input type=text>"),
              std::runtime_error
          );
          Element f1("<div id=\"123\">");
          assert(f1.m_Attributes[0].Value == "123");

          Element f2("<div id=\"hello-world\">");
          assert(f2.m_Attributes[0].Value == "hello-world");

          Element f3("<div id=\"hello_world\">");
          assert(f3.m_Attributes[0].Value == "hello_world");

          Element f4("<div id=\"a.b.c\">");
          assert(f4.m_Attributes[0].Value == "a.b.c");

          Element f5("<div id=\"123-456_test\">");
          assert(f5.m_Attributes[0].Value == "123-456_test");
          Element ell(
              "<div "
              "id=\"main\" "
              "class=\"container large\" "
              "data-id=\"12345\" "
              "aria-label=\"Main content\" "
              "title=\"Hello World\" "
              "role=\"main\" "
              "tabindex=\"0\">"
          );

          assert(ell.m_Name == "div");
          assert(ell.m_Attributes.size() == 7);

          assert(ell.m_Attributes[0].Name == "id");
          assert(ell.m_Attributes[0].Value == "main");

          assert(ell.m_Attributes[1].Name == "class");
          assert(ell.m_Attributes[1].Value == "container large");

          assert(ell.m_Attributes[2].Name == "data-id");
          assert(ell.m_Attributes[2].Value == "12345");

          assert(ell.m_Attributes[3].Name == "aria-label");
          assert(ell.m_Attributes[3].Value == "Main content");

          assert(ell.m_Attributes[4].Name == "title");
          assert(ell.m_Attributes[4].Value == "Hello World");

          assert(ell.m_Attributes[5].Name == "role");
          assert(ell.m_Attributes[5].Value == "main");

          assert(ell.m_Attributes[6].Name == "tabindex");
          assert(ell.m_Attributes[6].Value == "0");

          Element el("<div data-id=\"123\" aria-label=\"test\">");

          assert(el.m_Attributes[0].Name == "data-id");
          assert(el.m_Attributes[1].Name == "aria-label");
      }
      
};
