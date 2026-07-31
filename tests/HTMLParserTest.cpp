#include "crawler/HTMLParser.hpp"
#include <iostream>
#include <cassert>
#include <vector>
class HTMLParserTest {
    public:
        static void Run() {
            std::cout << "Testing HTML Parser: " << std::endl;
            test_HTMLParser();
            testMoreHTML();
            std::cout << "All tests passed!" << std::endl;
        }
    private:
    static void printElementsVector(std::vector<Element> vec) {
        for (const auto& e: vec) {
            std::cout << "Name: " << e.getName() << std::endl;
        }
    }
    static void test_HTMLParser() {
        // =========================================================
        // Basic parsing + callbacks
        // =========================================================

        HTMLParser parser;

        std::vector<Element> divElements;
        std::vector<Element> pElements;
        std::vector<Element> imgElements;
        std::vector<Element> brElements;
        std::vector<Element> titleElements;

        parser.registerCallback("div", [&](const Element& e) {
            divElements.push_back(e);
        });

        parser.registerCallback("p", [&](const Element& e) {
            pElements.push_back(e);
        });

        parser.registerCallback("img", [&](const Element& e) {
            imgElements.push_back(e);
        });

        parser.registerCallback("br", [&](const Element& e) {
            brElements.push_back(e);
        });

        parser.registerCallback("title", [&](const Element& e) {
            titleElements.push_back(e);
        });

        const std::string html = R"(
            <html>
                <head>
                    <title>My Page</title>
                </head>

                <body>
                    <div id="main" class="container">
                        <p>Hello world!</p>
                        <p class="second">Second paragraph</p>
                        <br>
                        <img src="image.jpg" alt="Example image">
                    </div>
                </body>
            </html>
        )";

        parser.parse(html);

        // =========================================================
        // title
        // =========================================================

        //std::cout << titleElements.size() << std::endl;
        //printElementsVector(titleElements);
        assert(titleElements.size() == 1);
        assert(titleElements[0].getName() == "title");
        assert(
            titleElements[0].m_Text.find("My Page")
            != std::string::npos
        );
        assert(titleElements[0].m_Attributes.empty());

        // =========================================================
        // div
        // =========================================================

        assert(divElements.size() == 1);
        assert(divElements[0].getName() == "div");

        assert(divElements[0].m_Attributes.size() == 2);

        assert(divElements[0].m_Attributes[0].Name == "id");
        assert(divElements[0].m_Attributes[0].Value == "main");

        assert(divElements[0].m_Attributes[1].Name == "class");
        assert(divElements[0].m_Attributes[1].Value == "container");

        // =========================================================
        // p
        // =========================================================

        assert(pElements.size() == 2);

        assert(pElements[0].getName() == "p");
        //std::cout << pElements[0].m_Text << std::endl;
        assert(
            pElements[0].m_Text.find("Hello world!")
            != std::string::npos
        );

        assert(pElements[0].m_Attributes.empty());

        assert(pElements[1].getName() == "p");
        assert(
            pElements[1].m_Text.find("Second paragraph")
            != std::string::npos
        );

        assert(pElements[1].m_Attributes.size() == 1);
        assert(pElements[1].m_Attributes[0].Name == "class");
        assert(pElements[1].m_Attributes[0].Value == "second");

        // =========================================================
        // br - void element
        // =========================================================

        assert(brElements.size() == 1);
        assert(brElements[0].getName() == "br");
        assert(brElements[0].m_Attributes.empty());

        // =========================================================
        // img - void element with attributes
        // =========================================================

        assert(imgElements.size() == 1);
        assert(imgElements[0].getName() == "img");

        assert(imgElements[0].m_Attributes.size() == 2);

        assert(imgElements[0].m_Attributes[0].Name == "src");
        assert(imgElements[0].m_Attributes[0].Value == "image.jpg");

        assert(imgElements[0].m_Attributes[1].Name == "alt");
        assert(imgElements[0].m_Attributes[1].Value == "Example image");

        // =========================================================
        // Multiple callbacks for same element
        // =========================================================

        HTMLParser callbackParser;

        int callback1 = 0;
        int callback2 = 0;

        callbackParser.registerCallback("p", [&](const Element&) {
            callback1++;
        });

        callbackParser.registerCallback("p", [&](const Element&) {
            callback2++;
        });
        callbackParser.parse(R"(
            <p>First</p>
            <p>Second</p>
            <p>Third</p>
        )"
        );

        //std::cout << callback1 << std::endl;
        assert(callback1 == 3);
        assert(callback2 == 3);

        // =========================================================
        // Callback should only run for registered elements
        // =========================================================

        HTMLParser selectiveParser;

        int pCount = 0;

        selectiveParser.registerCallback("p", [&](const Element&) {
            pCount++;
        });

        selectiveParser.parse(
            "<div>Hello</div>"
            "<span>Hello</span>"
            "<p>One</p>"
            "<div>World</div>"
            "<p>Two</p>"
        );

        //std::cout << pCount << std::endl;
        assert(pCount == 2);

        // =========================================================
        // Nested elements
        // =========================================================

        HTMLParser nestedParser;

        int nestedDivCount = 0;
        int nestedSpanCount = 0;

        nestedParser.registerCallback("div", [&](const Element& e) {
            nestedDivCount++;
            assert(e.getName() == "div");
        });

        nestedParser.registerCallback("span", [&](const Element& e) {
            nestedSpanCount++;
            assert(e.getName() == "span");
            assert(
                e.m_Text.find("Nested text")
                != std::string::npos
            );
        });

        nestedParser.parse(R"(
            <div>
                <span>Nested text</span>
            </div>
        )");

        assert(nestedDivCount == 1);
        assert(nestedSpanCount == 1);

        // =========================================================
        // Multiple top-level elements
        // =========================================================

        HTMLParser multiParser;

        std::vector<std::string> receivedTexts;

        multiParser.registerCallback("p", [&](const Element& e) {
            receivedTexts.push_back(e.m_Text);
        });

        multiParser.parse(R"(
            <p>First</p>
            <p>Second</p>
            <p>Third</p>
        )");

        assert(receivedTexts.size() == 3);

        assert(
            receivedTexts[0].find("First")
            != std::string::npos
        );

        assert(
            receivedTexts[1].find("Second")
            != std::string::npos
        );

        assert(
            receivedTexts[2].find("Third")
            != std::string::npos
        );

        // =========================================================
        // Empty element
        // =========================================================

        HTMLParser emptyParser;

        int emptyCount = 0;

        emptyParser.registerCallback("div", [&](const Element& e) {
            emptyCount++;

            assert(e.getName() == "div");
            assert(e.m_Attributes.empty());
            assert(e.m_Text.empty());
        });

        emptyParser.parse(R"(
            <div></div>
        )");

        assert(emptyCount == 1);

        // =========================================================
        // Several void elements
        // =========================================================

        HTMLParser voidParser;

        int voidCount = 0;

        voidParser.registerCallback("br", [&](const Element&) {
            voidCount++;
        });

        voidParser.registerCallback("hr", [&](const Element&) {
            voidCount++;
        });

        voidParser.registerCallback("img", [&](const Element&) {
            voidCount++;
        });

        voidParser.parse(R"(
            <div>
                <br>
                <hr>
                <img src="test.jpg">
            </div>
        )");

        assert(voidCount == 3);

        // =========================================================
        // Nothing registered
        // =========================================================

        HTMLParser noCallbackParser;

        // Should simply parse without invoking anything.
        noCallbackParser.parse(R"(
            <html>
                <body>
                    <p>Hello</p>
                </body>
            </html>
        )");

          
    }

    static void testMoreHTML() {

        // =========================================================
        // 1. Typical article page
        // =========================================================

        {
            HTMLParser parser;

            std::vector<Element> articles;
            std::vector<Element> headings;
            std::vector<Element> paragraphs;

            parser.registerCallback("article", [&](const Element& e) {
                articles.push_back(e);
            });

            parser.registerCallback("h1", [&](const Element& e) {
                headings.push_back(e);
            });

            parser.registerCallback("p", [&](const Element& e) {
                paragraphs.push_back(e);
            });
            parser.parse(R"(
                <!DOCTYPE html>
                <html lang="en">
                    <head>
                        <meta charset="UTF-8">
                        <title>Example News Article</title>
                    </head>
                    <body>
                        <main>
                            <article class="post" id="news-123">
                                <h1>New version of the product released</h1>

                                <p>
                                    The company announced a new version of its
                                    software today.
                                </p>

                                <p>
                                    The update includes improved performance,
                                    better security, and several bug fixes.
                                </p>
                            </article>
                        </main>
                    </body>
                </html>
            )");

            assert(articles.size() == 1);
            assert(articles[0].getName() == "article");

            assert(articles[0].m_Attributes.size() == 2);
            assert(articles[0].m_Attributes[0].Name == "class");
            assert(articles[0].m_Attributes[0].Value == "post");
            assert(articles[0].m_Attributes[1].Name == "id");
            assert(articles[0].m_Attributes[1].Value == "news-123");

            assert(headings.size() == 1);
            assert(
                headings[0].m_Text.find(
                    "New version of the product released"
                ) != std::string::npos
            );

            assert(paragraphs.size() == 2);
        }


        // =========================================================
        // 2. Navigation menu
        // =========================================================

        {
            HTMLParser parser;

            std::vector<Element> links;

            parser.registerCallback("a", [&](const Element& e) {
                links.push_back(e);
            });

            parser.parse(R"(
                <nav class="main-navigation">
                    <ul>
                        <li>
                            <a href="/">Home</a>
                        </li>
                        <li>
                            <a href="/products">Products</a>
                        </li>
                        <li>
                            <a href="/about">About us</a>
                        </li>
                        <li>
                            <a href="/contact">Contact</a>
                        </li>
                    </ul>
                </nav>
            )");

            assert(links.size() == 4);

            assert(links[0].m_Attributes[0].Name == "href");
            assert(links[0].m_Attributes[0].Value == "/");

            assert(
                links[0].m_Text.find("Home")
                != std::string::npos
            );

            assert(
                links[1].m_Text.find("Products")
                != std::string::npos
            );

            assert(
                links[2].m_Text.find("About us")
                != std::string::npos
            );

            assert(
                links[3].m_Text.find("Contact")
                != std::string::npos
            );
        }


        // =========================================================
        // 3. Realistic form
        // =========================================================

        {
            HTMLParser parser;

            std::vector<Element> inputs;

            parser.registerCallback("input", [&](const Element& e) {
                inputs.push_back(e);
            });

            parser.parse(R"(
                <form action="/login" method="post">
                    <label for="email">Email</label>
                    <input
                        type="email"
                        id="email"
                        name="email"
                        placeholder="you@example.com"
                    >

                    <label for="password">Password</label>
                    <input
                        type="password"
                        id="password"
                        name="password"
                    >

                    <input
                        type="submit"
                        value="Sign in"
                    >
                </form>
            )");

            assert(inputs.size() == 3);

            assert(inputs[0].getName() == "input");
            assert(inputs[0].m_Attributes.size() == 4);

            assert(inputs[1].getName() == "input");
            assert(inputs[1].m_Attributes.size() == 3);

            assert(inputs[2].getName() == "input");
            assert(inputs[2].m_Attributes.size() == 2);
        }


        // =========================================================
        // 4. Images and links
        // =========================================================

        {
            HTMLParser parser;

            std::vector<Element> images;
            std::vector<Element> links;

            parser.registerCallback("img", [&](const Element& e) {
                images.push_back(e);
            });

            parser.registerCallback("a", [&](const Element& e) {
                links.push_back(e);
            });

            parser.parse(R"(
                <div class="gallery">
                    <a href="/images/1">
                        <img
                            src="/static/images/photo1.jpg"
                            alt="Mountain landscape"
                            width="800"
                            height="600"
                        >
                    </a>

                    <a href="/images/2">
                        <img
                            src="/static/images/photo2.jpg"
                            alt="Ocean sunset"
                        >
                    </a>
                </div>
            )");

            assert(images.size() == 2);
            assert(links.size() == 2);

            assert(
                images[0].m_Attributes[0].Name == "src"
            );

            assert(
                images[0].m_Attributes[0].Value
                == "/static/images/photo1.jpg"
            );

            assert(
                images[0].m_Attributes[1].Name == "alt"
            );

            assert(
                images[0].m_Attributes[1].Value
                == "Mountain landscape"
            );
        }


        // =========================================================
        // 5. Table
        // =========================================================

        {
            HTMLParser parser;

            std::vector<Element> rows;
            std::vector<Element> cells;

            parser.registerCallback("tr", [&](const Element& e) {
                rows.push_back(e);
            });

            parser.registerCallback("td", [&](const Element& e) {
                cells.push_back(e);
            });

            parser.parse(R"(
                <table class="users">
                    <thead>
                        <tr>
                            <th>Name</th>
                            <th>Email</th>
                        </tr>
                    </thead>

                    <tbody>
                        <tr>
                            <td>Alice</td>
                            <td>alice@example.com</td>
                        </tr>

                        <tr>
                            <td>Bob</td>
                            <td>bob@example.com</td>
                        </tr>
                    </tbody>
                </table>
            )");

            assert(rows.size() == 3);
            assert(cells.size() == 4);

            assert(
                cells[0].m_Text.find("Alice")
                != std::string::npos
            );

            assert(
                cells[1].m_Text.find("alice@example.com")
                != std::string::npos
            );

            assert(
                cells[2].m_Text.find("Bob")
                != std::string::npos
            );

            assert(
                cells[3].m_Text.find("bob@example.com")
                != std::string::npos
            );
        }


        // =========================================================
        // 6. Metadata / head section
        // =========================================================

        {
            HTMLParser parser;

            std::vector<Element> meta;
            std::vector<Element> links;

            parser.registerCallback("meta", [&](const Element& e) {
                meta.push_back(e);
            });

            parser.registerCallback("link", [&](const Element& e) {
                links.push_back(e);
            });

            parser.parse(R"(
                <head>
                    <meta charset="UTF-8">
                    <meta
                        name="description"
                        content="A test website"
                    >
                    <meta
                        name="viewport"
                        content="width=device-width, initial-scale=1.0"
                    >

                    <link
                        rel="stylesheet"
                        href="/css/main.css"
                    >

                    <link
                        rel="icon"
                        href="/favicon.ico"
                    >
                </head>
            )");

            assert(meta.size() == 3);
            assert(links.size() == 2);

            assert(meta[0].m_Attributes[0].Name == "charset");
            assert(meta[0].m_Attributes[0].Value == "UTF-8");

            assert(meta[1].m_Attributes[0].Name == "name");
            assert(meta[1].m_Attributes[0].Value == "description");

            assert(
                meta[1].m_Attributes[1].Name == "content"
            );

            assert(
                meta[1].m_Attributes[1].Value
                == "A test website"
            );
        }


        // =========================================================
        // 7. Deeply nested real-world structure
        // =========================================================

        {
            HTMLParser parser;

            int articleCount = 0;
            int sectionCount = 0;
            int spanCount = 0;

            parser.registerCallback("article", [&](const Element&) {
                articleCount++;
            });

            parser.registerCallback("section", [&](const Element&) {
                sectionCount++;
            });

            parser.registerCallback("span", [&](const Element&) {
                spanCount++;
            });

            parser.parse(R"(
                <article class="product">
                    <section class="header">
                        <div class="title">
                            <h1>
                                <span>Awesome Product</span>
                            </h1>
                        </div>
                    </section>

                    <section class="content">
                        <div class="price">
                            <span class="currency">$</span>
                            <span class="amount">49.99</span>
                        </div>

                        <div class="description">
                            <p>
                                This is a great product with many features.
                            </p>
                        </div>
                    </section>
                </article>
            )");

            assert(articleCount == 1);
            assert(sectionCount == 2);
            assert(spanCount == 3);
        }


        // =========================================================
        // 8. Whitespace-heavy HTML
        // =========================================================

        {
            HTMLParser parser;

            std::vector<Element> paragraphs;

            parser.registerCallback("p", [&](const Element& e) {
                paragraphs.push_back(e);
            });

            parser.parse(R"(

                <div>


                    <p>
                        Hello
                        world
                    </p>



                    <p>
                        Another
                        paragraph.
                    </p>


                </div>

            )");

            assert(paragraphs.size() == 2);

            assert(
                paragraphs[0].m_Text.find("Hello")
                != std::string::npos
            );

            assert(
                paragraphs[0].m_Text.find("world")
                != std::string::npos
            );

            assert(
                paragraphs[1].m_Text.find("Another")
                != std::string::npos
            );
        }


        // =========================================================
        // 9. Article with inline formatting
        // =========================================================

        {
            HTMLParser parser;

            std::vector<Element> strong;
            std::vector<Element> emphasis;
            std::vector<Element> code;

            parser.registerCallback("strong", [&](const Element& e) {
                strong.push_back(e);
            });

            parser.registerCallback("em", [&](const Element& e) {
                emphasis.push_back(e);
            });

            parser.registerCallback("code", [&](const Element& e) {
                code.push_back(e);
            });

            parser.parse(R"(
                <p>
                    This is
                    <strong>very important</strong>
                    text with
                    <em>emphasis</em>
                    and
                    <code>std::string</code>.
                </p>
            )");

            assert(strong.size() == 1);
            assert(emphasis.size() == 1);
            assert(code.size() == 1);

            assert(
                strong[0].m_Text.find("very important")
                != std::string::npos
            );

            assert(
                emphasis[0].m_Text.find("emphasis")
                != std::string::npos
            );

            assert(
                code[0].m_Text.find("std::string")
                != std::string::npos
            );
        }


        // =========================================================
        // 10. HTML with special characters in attributes
        // =========================================================

        {
            HTMLParser parser;

            std::vector<Element> elements;

            parser.registerCallback("div", [&](const Element& e) {
                elements.push_back(e);
            });

            parser.parse(R"(
                <div
                    id="user-123"
                    class="card card--featured"
                    data-user-id="98765"
                    aria-label="User's profile"
                    title="Hello, world!"
                >
                    Profile
                </div>
            )");

            assert(elements.size() == 1);

            assert(elements[0].m_Attributes.size() == 5);

            assert(
                elements[0].m_Attributes[0].Name == "id"
            );

            assert(
                elements[0].m_Attributes[0].Value == "user-123"
            );

            assert(
                elements[0].m_Attributes[1].Name == "class"
            );

            assert(
                elements[0].m_Attributes[1].Value
                == "card card--featured"
            );

            assert(
                elements[0].m_Attributes[2].Name == "data-user-id"
            );

            assert(
                elements[0].m_Attributes[2].Value == "98765"
            );
        }
    }
};
