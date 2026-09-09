#include<bits/stdc++.h>
using namespace std;

class DocumentElement{
public:
    virtual string render() = 0;
};

class TextElement : public DocumentElement{
private:
    string text;
public:
    TextElement(string text){
        this->text = text;
    }

    string render() override{
        return "Rendering text: " + this->text;
    }
};

class ImageElement : public DocumentElement{
private:
    string text;
public:
    ImageElement(string text){
        this->text = text;
    }

    string render() override{
        return "Rendering Image: " + this->text;
    }
};

class NewLineElement : public DocumentElement{
public:
    string render() override{
        return "\n";
    }
};

class Document{
private:
    vector<DocumentElement*> document;
public:
    void addElement(DocumentElement* doc){
        document.push_back(doc);
    }

    string renderDoc(){
        string res;
        for(auto &it: document){
            res += it->render();
        }

        return res;
    }
};

class Persistent{
public:
    virtual void save(string data) = 0;
};

class SaveFile : public Persistent{
public:
    void save(string data) override{
        // ofstream outFile("document.txt");
        // if (outFile) {
        //     outFile << data;
        //     outFile.close();
        //     cout << "Document saved to document.txt" << endl;
        // } else {
        //     cout << "Error: Unable to open file for writing." << endl;
        // }
        cout << "Document saved to document.txt" << endl;
    }
};

class DBStorage : public Persistent {
public:
    void save(string data) override {
        cout << "Document saved to DB" << endl;
    }
};


class DocumentEditor{
private:
    Document *doc;
    Persistent *per;
    string renderedDoc;
public:
    DocumentEditor(Document* doc, Persistent* per){
        this->doc = doc;
        this->per = per;
    }

    void addText(string s){
        doc->addElement(new TextElement(s));
    }

    void addImage(string imagePath) {
        doc->addElement(new ImageElement(imagePath));
    }

    void addNewLine() {
        doc->addElement(new NewLineElement());
    }

    string renderDoc(){
        if(renderedDoc.empty()){
            renderedDoc = doc->renderDoc();
        }
        return renderedDoc;
    }

    void save(){
        per->save(renderDoc());
    }
};

int main(){
    Document *doc = new Document();
    Persistent *per = new SaveFile();
    DocumentEditor* editor = new DocumentEditor(doc, per);

    editor->addText("Hello World");
    editor->addNewLine();
    editor->addText("This is a real-world document editor example.");
    editor->addNewLine();
    editor->addText("Indented text after a tab space.");
    editor->addNewLine();
    editor->addImage("picture.jpg");

    cout << editor->renderDoc() << endl;
    editor->save();
}