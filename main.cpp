#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <string>
#include <vector>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <unordered_map>
#include <iostream>
using namespace std;
#include <QMessageBox>

void showErrorMessage(const QString &message) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);  // Set the icon to 'Critical' for errors
    msgBox.setWindowTitle("Error");  // Set the window title
    msgBox.setText(message);  // Set the message text
    msgBox.exec();  // Display the message box
}
// Enum and Token struct definitions
typedef enum {
    SEMICOLON, IF, THEN, END, REPEAT, UNTIL, READ, WRITE,
    ASSIGN, LESSTHAN, EQUAL, PLUS, MINUS, MULT, DIV,
    OPENBRACKET, CLOSEBRACKET, IDENTIFIER, NUMBER,
    UNKNOWN, ELSE
} TokenType;

typedef struct {
    std::string value;
    TokenType type;
} Token;

// Function to map string to TokenType
TokenType stringToTokenType(const QString &str) {
    static std::unordered_map<QString, TokenType> tokenMap = {
        {"SEMICOLON", SEMICOLON}, {"IF", IF}, {"THEN", THEN}, {"END", END},
        {"REPEAT", REPEAT}, {"UNTIL", UNTIL}, {"READ", READ}, {"WRITE", WRITE},
        {"ASSIGN", ASSIGN}, {"LESSTHAN", LESSTHAN}, {"EQUAL", EQUAL}, {"PLUS", PLUS},
        {"MINUS", MINUS}, {"MULT", MULT}, {"DIV", DIV}, {"OPENBRACKET", OPENBRACKET},
        {"CLOSEBRACKET", CLOSEBRACKET}, {"IDENTIFIER", IDENTIFIER}, {"NUMBER", NUMBER},
        {"UNKNOWN", UNKNOWN}, {"ELSE", ELSE}
    };

    auto it = tokenMap.find(str);
    return (it != tokenMap.end()) ? it->second : UNKNOWN;
}

// Function to process tokens from input text
std::vector<std::pair<std::string, TokenType>> processTokens(const QString &text) {
    std::vector<std::pair<std::string, TokenType>> tokens;

    // Split text into lines
    QStringList lines = text.split("\n", Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        // Split each line into parts by ','
        QStringList parts = line.split(",", Qt::SkipEmptyParts);
        if (parts.size() == 2) {
            std::string value = parts[0].toStdString();
            TokenType type = stringToTokenType(parts[1].trimmed());
            tokens.emplace_back(value, type); // Add pair to vector
        }
    }

    return tokens;
}
string tokenTypeToString(TokenType type) {
    switch (type) {
    case SEMICOLON: return "SEMICOLON";
    case IF: return "IF";
    case THEN: return "THEN";
    case END: return "END";
    case REPEAT: return "REPEAT";
    case UNTIL: return "UNTIL";
    case READ: return "READ";
    case WRITE: return "WRITE";
    case ASSIGN: return "ASSIGN";
    case LESSTHAN: return "LESSTHAN";
    case EQUAL: return "EQUAL";
    case PLUS: return "PLUS";
    case MINUS: return "MINUS";
    case MULT: return "MULT";
    case DIV: return "DIV";
    case OPENBRACKET: return "OPENBRACKET";
    case CLOSEBRACKET: return "CLOSEBRACKET";
    case IDENTIFIER: return "IDENTIFIER";
    case NUMBER: return "NUMBER";
    case UNKNOWN: return "UNKNOWN";
    case ELSE: return "ELSE";
    default: return "UNKNOWN";
    }
}
struct syntax_tree_node {
    int id; // Unique ID for each node
    string label; // Node label (e.g., "if", "repeat", "assign")
    vector<syntax_tree_node*> children; // Edges to child nodes

    syntax_tree_node(int id, const string& label) : id(id), label(label) {}

    ~syntax_tree_node() {
        for (auto child : children) {
            delete child;
        }
    }

    void addChild(syntax_tree_node* child) {
        children.push_back(child);
        // cout <<child->id <<" "<< child->label<<endl;
    }
};

class Parser {
private:
    vector<pair<string, TokenType>> tokens;
    size_t current; // current position in tokens
    syntax_tree_node* root; // Root of the syntax tree
    int currentNodeID; //giving each node an ID to ease visualising it , you'll see ...


    // Get the current token
    pair<string, TokenType> peek() const {
        if (current < tokens.size()) {
            return tokens[current];
        }
        return {"", UNKNOWN}; // Return an UNKNOWN token at EOF
    }

    // Advance to the next token
    pair<string, TokenType> advance() {
        if (current < tokens.size()) {
            return tokens[current++];
        }
        return {"", UNKNOWN}; // Return an UNKNOWN token at EOF
    }

    // Match the current token with the expected type
    void match(TokenType expectedType) {
        if (peek().second == expectedType) {
            advance();
        } else {
            showErrorMessage("user used unexpected type");
            fprintf(stderr, "user used unexpected type");
        }
    }
    syntax_tree_node* program(syntax_tree_node* parent){

        //cout<<"program"<<endl;

        return stmt_sequence(parent);
    }
    syntax_tree_node* stmt_sequence(syntax_tree_node* parent){
        //cout<<"stmtseq"<<endl;
        syntax_tree_node* stmt_seq_node = new syntax_tree_node(currentNodeID++,"stmt_seq");
        //parent->addChild(stmt_seq_node);

        do {
            syntax_tree_node* stmt_node = statement(parent);
            parent->addChild(stmt_node);
            if (peek().second == SEMICOLON) {
                match(SEMICOLON);
            } else {
                break;
            }
        } while (true);

        return stmt_seq_node;
    }
    syntax_tree_node* statement(syntax_tree_node* parent){

        syntax_tree_node* stmt_node = new syntax_tree_node(currentNodeID++,"statement");
        cout<<peek().first<<endl;
        if (peek().second == IF) {
            syntax_tree_node* node = if_stmt(stmt_node);
            //parent->addChild(node);
        } else if (peek().second == REPEAT) {
            //cout<<"repeat"<<endl;
            syntax_tree_node* node = repeat_stmt(stmt_node);
            parent->addChild(node);
        } else if (peek().second == IDENTIFIER) {
            //cout<<"assn"<<endl;
            syntax_tree_node* node = assign_stmt(stmt_node);
            parent->addChild(node);
        } else if (peek().second == READ) {
            syntax_tree_node* node = read_stmt(stmt_node);
            parent->addChild(node);
        } else if (peek().second == WRITE) {
            syntax_tree_node* node = write_stmt(stmt_node);
            parent->addChild(node);
        } else {
            showErrorMessage("Syntax Error: Invalid statement.");
            throw runtime_error("Syntax Error: Invalid statement.");
        }
        return stmt_node;
    }
    syntax_tree_node* if_stmt(syntax_tree_node* parent)
    {
        syntax_tree_node* if_node = new syntax_tree_node(currentNodeID++,"if-stmt");
        parent->addChild(if_node);
        //cout<<"ifstmt"<<endl;
        match(IF);
        syntax_tree_node* exp_node=exp(if_node);
        if_node->addChild(exp_node);
        match(THEN);

        syntax_tree_node* stmt_seq_node =stmt_sequence(if_node);
        parent->addChild(stmt_seq_node);
        if(peek().second== ELSE)
        {
            syntax_tree_node* else_node = new syntax_tree_node(currentNodeID++,"else");
            parent->addChild(else_node);
            match(ELSE);
            syntax_tree_node* stmt_seq_node = stmt_sequence(else_node);
            else_node->addChild(stmt_seq_node);
        }
        match(END);
        return if_node;
    }

    syntax_tree_node* repeat_stmt(syntax_tree_node* parent)
    {
        syntax_tree_node* repeat_stmt = new syntax_tree_node(currentNodeID++,"repeat");
        parent->addChild(repeat_stmt);
        //cout<<"repeat"<<endl;
        match(REPEAT);

        syntax_tree_node* stmt_sequence_node = stmt_sequence(repeat_stmt);
        repeat_stmt->addChild(stmt_sequence_node);
        match(UNTIL);

        syntax_tree_node* exp_node =exp(repeat_stmt);
        repeat_stmt->addChild(exp_node);
        return repeat_stmt;
    }

    syntax_tree_node* assign_stmt(syntax_tree_node* parent)
    {
        syntax_tree_node* assign_stmt = new syntax_tree_node(currentNodeID++,":=");
        parent->addChild(assign_stmt);
        //cout<<"assn"<<endl;
        syntax_tree_node* id_node = new syntax_tree_node(currentNodeID++,peek().first);

        match(IDENTIFIER);
        assign_stmt->addChild(id_node);
        match(ASSIGN);
        syntax_tree_node* exp_node= exp(assign_stmt);
        return assign_stmt;
    }

    syntax_tree_node* read_stmt(syntax_tree_node* parent)
    {

        //cout<<"read"<<endl;
        match(READ);

        syntax_tree_node* read_stmt = new syntax_tree_node(currentNodeID++,"read ("+peek().first+")");
        //parent->addChild(read_stmt);
        match(IDENTIFIER);

        return read_stmt;
    }

    syntax_tree_node* write_stmt(syntax_tree_node* parent)
    {
        syntax_tree_node* write_node = new syntax_tree_node(currentNodeID++,"write");
        //parent->addChild(write_node);
        //cout<<"write"<<endl;
        match(WRITE);
        syntax_tree_node* exp_node = exp(write_node);
        return write_node;
    }

    syntax_tree_node* exp(syntax_tree_node* parent)
    {
        //syntax_tree_node* exp_node=new syntax_tree_node(currentNodeID++,tokens.at(current+1).first);
        //cout<<"exp"<<endl;
        //cout<<parent->label<<endl;
        //parent->addChild(exp_node);
        syntax_tree_node* simple_exp_node =simple_exp(parent);
        parent->addChild(simple_exp_node);
        if(peek().second == LESSTHAN | peek().second == EQUAL){
            match(peek().second);
            syntax_tree_node* simple_exp2= simple_exp(parent);
            parent->addChild(simple_exp2);
        }
        return parent;
    }

    syntax_tree_node* simple_exp(syntax_tree_node* parent)
    {
        syntax_tree_node* smplexp = new syntax_tree_node(currentNodeID++,tokens.at(current+1).first);
        //parent->addChild(smplexp);
        //cout<<"simpleexp\n";

        syntax_tree_node* term_node =term(smplexp);
        //smplexp->addChild(term_node);

        while(peek().second == PLUS | peek().second == MINUS){
            syntax_tree_node* addop=new syntax_tree_node(currentNodeID++,peek().first);
            match(peek().second);
            syntax_tree_node* term_node2 =term(smplexp);
            smplexp->addChild(term_node2);
        }
        return smplexp;
    }

    syntax_tree_node* term(syntax_tree_node* parent)
    {   syntax_tree_node* op_node = new syntax_tree_node(currentNodeID++, peek().first);
        //parent->addChild(op_node);
        //cout<<"term\n";
        //syntax_tree_node* op_node;

        do{
            //syntax_tree_node* fac_node =factor(parent);
            //parent->addChild(fac_node);
            //cout<<peek().first<<endl;
            factor(parent);

            if(peek().second == MULT | peek().second == DIV){
                op_node=new syntax_tree_node(currentNodeID++,peek().first);
                parent->addChild(op_node);
                op_node->addChild(op_node);
                match(peek().second);
                //syntax_tree_node* fac_node2 = factor(parent);
                factor(parent);

            }

        }while(peek().second == MULT | peek().second == DIV);
        return op_node;
    }

    void factor(syntax_tree_node* parent)
    {
        //cout<<"factor\n";
        if(peek().second == OPENBRACKET){
            match(OPENBRACKET);
            exp(parent);
            //syntax_tree_node* node =exp(parent);
            match(CLOSEBRACKET);
            //return node;
        }
        else if(peek().second == NUMBER){
            // syntax_tree_node* node = new syntax_tree_node(currentNodeID++,"number");
            // parent->addChild(node);
            match(peek().second);
            //return node;
        }
        else if(peek().second == IDENTIFIER) {
            // syntax_tree_node* node = new syntax_tree_node(currentNodeID++,"id");
            //parent->addChild(node);
            cout<<peek().first<<endl;
            match(peek().second);
            // return node;
        }
        else{
            showErrorMessage("Syntax Error: Invalid statement.");
            throw runtime_error("Syntax Error: Invalid statement.");}


    }
public:
    // Constructor to initialize the parser with tokens
    Parser(const vector<pair<string, TokenType>>& tokens)
        : tokens(tokens), current(0), root(nullptr), currentNodeID(0) {}
    syntax_tree_node* parse() {
        syntax_tree_node* root = program(root);
        return root;
    }


};


// Function to recursively render a syntax tree
void renderGraph(QGraphicsScene* scene,syntax_tree_node* node, qreal x, qreal y, qreal xOffset, qreal yOffset) {
    if (!node) {
        std::cerr << "Node is null! Skipping..." << std::endl;
        return;
    }

    std::cerr << "Rendering node: " << node->label << " at (" << x << ", " << y << ")" << std::endl;

    // Draw the current node
    QGraphicsEllipseItem* circle = scene->addEllipse(x - 20, y - 20, 40, 40, QPen(Qt::black), QBrush(Qt::white));
    QGraphicsTextItem* text = scene->addText(QString::fromStdString(node->label));
    text->setDefaultTextColor(Qt::black);
    text->setPos(x - text->boundingRect().width() / 2, y - text->boundingRect().height() / 2);

    // Draw edges and recursively render children
    for (size_t i = 0; i < node->children.size(); ++i) {
        qreal childX;
        if (node->children.size() == 1) {
            childX = x; // Center single child
        } else {
            childX = x - xOffset + (2 * xOffset * i / (node->children.size() - 1));
        }
        qreal childY = y + yOffset;

        std::cerr << "Connecting " << node->label<< " to child: " << node->children[i]->label
                  << " at (" << childX << ", " << childY << ")" << std::endl;

        scene->addLine(x, y + 20, childX, childY - 20, QPen(Qt::black));
        renderGraph(scene, node->children[i], childX, childY, xOffset / 2, yOffset);
    }
}
void printTree(syntax_tree_node* root, int level = 0) {
    for (int i = 0; i < level; ++i) {
        cout << "  "; // Indentation for each level
    }
    cout << root->label << endl;

    for (syntax_tree_node* child : root->children) {
        printTree(child, level + 1);
    }
}
// Main function with a Qt Text Editor
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    // Create the main window widget
    QWidget view;

    // Create the main horizontal layout (QHBoxLayout)
    QHBoxLayout *hLayout = new QHBoxLayout(&view);

    // Create the vertical layout (QVBoxLayout) for the button and text editor
    QVBoxLayout *vLayout = new QVBoxLayout();

    // Create a push button and a text editor
    QPushButton *button = new QPushButton("generate syntax tree!");
    QTextEdit *textEdit = new QTextEdit();
    textEdit->setPlaceholderText("Enter tokens in 'value,TOKENTYPE' format, one per line...");
    // Add the button and text editor to the vertical layout
    vLayout->addWidget(button);
    vLayout->addWidget(textEdit);

    // Create a QGraphicsView and its associated scene
    QGraphicsView *graphicsView = new QGraphicsView();
    QGraphicsScene *scene = new QGraphicsScene();
    graphicsView->setScene(scene);
    scene->setSceneRect(0, 0, 200, 200); // Set the scene size for the graphics view

    // Add the vertical layout (button + text editor) and graphics view to the horizontal layout
    hLayout->addLayout(vLayout);
    hLayout->addWidget(graphicsView);

    // Set the stretch factors to control the ratio
    hLayout->setStretchFactor(vLayout, 2); // Left side (button + text editor) takes 2 parts
    hLayout->setStretchFactor(graphicsView, 3); // Right side (graphics view) takes 3 parts
    QObject::connect(button, &QPushButton::clicked, [&]() {
        QString inputText = textEdit->toPlainText();
        std::vector<pair<string, TokenType>> tokens = processTokens(inputText);
        Parser parser(tokens);

        syntax_tree_node* root = parser.parse();
        QGraphicsTextItem *textItem = new QGraphicsTextItem("PARSING SUCCEEDED!");

        // Customize the text (set font, color, etc.)
        QFont font("Arial", 16);
        textItem->setFont(font);
        textItem->setDefaultTextColor(Qt::black);  // Set the text color to black

        // Set the position of the text
        textItem->setPos(50, 50);
        scene->addItem(textItem);
        // Add the text item to the scene


        //printTree(root);
        //renderGraph(scene, root, 400, 50, 200, 100);
        delete root;
    });
    // Set the main window properties


    // Create a QGraphicsScene and QGraphicsView to display the tree
    // renderGraph(scene, root, 400, 50, 200, 100);


    // view.setRenderHint(QPainter::Antialiasing);
    view.setWindowTitle("Syntax Tree Visualization");
    view.resize(800, 600);
    view.show();

    // Run the application
    return app.exec();
}


