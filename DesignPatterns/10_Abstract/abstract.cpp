#include<bits/stdc++.h>
using namespace std;

class ModelTrainer{
public:
    void trainPipeline(const string& dataPath) {
        loadData(dataPath);
        preprocessData();
        trainModel();      
        evaluateModel();   
        saveModel();       
    }

protected:
    void loadData(const string& path) {
        cout << "[Common] Loading dataset from " << path << "\n";
    }
    virtual void preprocessData() {
        cout << "[Common] Splitting into train/test and normalizing\n";
    }
    virtual void trainModel() = 0;
    virtual void evaluateModel() = 0;
    virtual void saveModel() {
        cout << "[Common] Saving model to disk as default format\n";
    }
};


class NeuralNetworkTrainer : public ModelTrainer {
protected:
    void trainModel() override {
        cout << "[NeuralNet] Training Neural Network for 100 epochs\n";
    }
    void evaluateModel() override {
        cout << "[NeuralNet] Evaluating accuracy and loss on validation set\n";
    }
    void saveModel() override {
        cout << "[NeuralNet] Serializing network weights to .h5 file\n";
    }
};


class DecisionTreeTrainer : public ModelTrainer {
protected:
    void trainModel() override {
        cout << "[DecisionTree] Building decision tree with max_depth=5\n";
    }
    void evaluateModel() override {
        cout << "[DecisionTree] Computing classification report (precision/recall)\n";
    }
    // use the default saveModel()
};

int main(){
    cout << "=== Neural Network Training ===\n";
    ModelTrainer* nnTrainer = new NeuralNetworkTrainer();
    nnTrainer->trainPipeline("data/images/");

    cout << "\n=== Decision Tree Training ===\n";
    ModelTrainer* dtTrainer = new DecisionTreeTrainer();
    dtTrainer->trainPipeline("data/iris.csv");
}