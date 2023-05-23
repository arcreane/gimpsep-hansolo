#include <iostream>
#include <opencv2/opencv.hpp>
#include <filesystem>

using namespace std;
using namespace cv;

void printWelcomeMessage();
void printErrorMessage();
void printGoodbyeMessage();
void printModeSelection();
void printFileSelection();
void printDirectorySelection();
void printFunctionalitySelectionForSimple(const string& filename);
void printFunctionalitySelectionForMultiple(const string& directory);
void onSizeMorphologicalOperationChange(int value, void* userData);
Mat applyMorphologicalOperation(const Mat& source_image, int size, const string& operation);
void morphologicalOperation(const string& filename, string operation);
void onFactorChange(int value, void* userData);
Mat applyBrightnessOperation(const Mat& source_image, int factor, const string& operation);
void brightnessOperation(const string& filename, string operation);
void onSizeCannyEdgeDetectionChange(int value, void* userData);
Mat applyCannyEdgeDetection(const Mat& source_image, int threshold1);
void cannyEdgeDetection(const string& filename);
void printInputTypeSelection(const string& filename);
void printResizingOperationWithFactor(const string& filename);
void resizingOperationWithFactor(const string& filename, double factor, const string& operation);
Mat applyResizingOperationWithFactor(const Mat& source_image, double factor, const string& operation);
void printResizingOperationTwoDimensions(const string& filename);
void resizingOperationWithTwoDimensions(const string& filename, int height, int width);
Mat applyResizingOperationWithTwoDimensions(const Mat& source_image, int height, int width);
void stitchingOperation(const string &directory);

int main() {
    freopen("/dev/null", "w", stderr);
    printWelcomeMessage();
    printGoodbyeMessage();
    return 0;
}

void printWelcomeMessage() {
    cout << endl;
    cout << "Welcome " << "\U0001F600" << " I am a command line image editor !" << endl;
    printModeSelection();
}

void printErrorMessage() {
    cout << endl;
    cout << "Oh " << "\U0001F62C" << " There is an error !" << endl;
}

void printGoodbyeMessage() {
    cout << endl;
    cout << "Goodbye " << "\U0001F618" << " It was a pleasure serving you !" << endl;
}

void printModeSelection() {
    int mode;
    cout << endl;
    cout << "Which mode do you want to use ?" << endl;
    cout << "(1) Simple" << endl;
    cout << "(2) Multiple" << endl;
    cout << "[3] Exit" << endl;
    cout << endl;
    cout << " > ";
    cin >> mode;
    switch (mode) {
        case 1:
            printFileSelection();
            break;
        case 2:
            printDirectorySelection();
            break;
        case 3:
            break;
        default:
            printErrorMessage();
    }
}

void printFileSelection() {
    while (true) {
        cout << endl;
        cout << "Which image do you want to edit ?" << endl;
        string directory = "/mnt/c/Users/alexa/OneDrive - ISEP/Documents/A2/Applications Multimédias/Projet/Rendu/Code/Original Images/";
        vector<string> filenames;
        glob(directory + "*.jpg", filenames);
        int image_index;
        int i = 1;
        for (const auto& filename : filenames) {
            filesystem::path path(filename);
            cout << "(" << i << ") " << path.filename().string() << endl;
            i++;
        }
        cout << "[" << i << "] " << "Go back" << endl;
        cout << endl;
        cout << " > ";
        cin >> image_index;
        if(image_index < i) {
            printFunctionalitySelectionForSimple(filenames[image_index-1]);
        } else if (image_index == i) {
            printModeSelection();
            break;
        } else {
            printErrorMessage();
            break;
        }
    }
}

void printDirectorySelection() {
    while (true) {
        cout << endl;
        cout << "Which folder do you want to use?" << endl;
        string home = "/mnt/c/Users/alexa/OneDrive - ISEP/Documents/A2/Applications Multimédias/Projet/Rendu/Code/Original Images/";
        vector<string> directories;
        int directory_index;
        int i = 1;
        for (const auto& entry : filesystem::directory_iterator(home)) {
            if (filesystem::is_directory(entry.path())) {
                directories.push_back(entry.path().string());
                cout << "(" << i << ") " << entry.path().filename().string() << endl;
                i++;
            }
        }
        cout << "[" << i << "] " << "Go back" << endl;
        cout << endl;
        cout << " > ";
        cin >> directory_index;
        if (directory_index < i) {
            printFunctionalitySelectionForMultiple(directories[directory_index-1]);
        } else if (directory_index == i) {
            printModeSelection();
            break;
        } else {
            printErrorMessage();
            break;
        }
    }
}

void printFunctionalitySelectionForSimple(const string& filename) {
    int functionality_index;
    cout << endl;
    cout << "What functionality do you want to use ?" << endl;
    cout << "(1) Dilation" << endl;
    cout << "(2) Erosion" << endl;
    cout << "(3) Resize" << endl;
    cout << "(4) Lighten" << endl;
    cout << "(5) Darken" << endl;
    cout << "(6) Canny Edge Detection" << endl;
    cout << "[7] Go back" << endl;
    cout << endl;
    cout << " > ";
    cin >> functionality_index;
    switch (functionality_index) {
        case 1:
            morphologicalOperation(filename, "Dilated");
            break;
        case 2:
            morphologicalOperation(filename, "Eroded");
            break;
        case 3:
            printInputTypeSelection(filename);
            break;
        case 4:
            brightnessOperation(filename, "Lighted");
            break;
        case 5:
            brightnessOperation(filename, "Darked");
            break;
        case 6:
            cannyEdgeDetection(filename);
            break;
        case 7:
            break;
        default:
            printErrorMessage();
    }
}

void printFunctionalitySelectionForMultiple(const string &directory) {
    int functionality_index;
    cout << endl;
    cout << "What functionality do you want to use ?" << endl;
    cout << "(1) Stitching" << endl;
    cout << "[2] Go back" << endl;
    cout << endl;
    cout << " > ";
    cin >> functionality_index;
    switch (functionality_index) {
        case 1:
            stitchingOperation(directory);
            break;
        case 2:
            printDirectorySelection();
            break;
        default:
            printErrorMessage();
    }
}


struct MorphologicalOperationParameters {
    Mat* source_image;
    int* kernel_size;
    String* operation;
};

void onSizeMorphologicalOperationChange(int kernel_size_value, void* userData) {
    auto* parameters = static_cast<MorphologicalOperationParameters*>(userData);
    Mat* source_image = parameters->source_image;
    int* kernel_size = parameters->kernel_size;
    String operation = *(parameters->operation);
    *kernel_size = kernel_size_value;
    if (*kernel_size > 0) {
        imshow("Press C to close and S to save", applyMorphologicalOperation(*source_image, *kernel_size, operation));
    }
}

Mat applyMorphologicalOperation(const Mat& source_image, int kernel_size, const String& operation) {
    Mat edited_image;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(kernel_size, kernel_size));
    if (operation == "Eroded") {
        erode(source_image, edited_image, kernel);
    } else if (operation == "Dilated") {
        dilate(source_image, edited_image, kernel);
    }
    return edited_image;
}

void morphologicalOperation(const string& filename, String operation) {
    Mat source_image = imread(filename);
    if (source_image.empty()) {
        printErrorMessage();
    } else {
        filesystem::path path(filename);
        String source_image_filename = path.filename().string();
        int kernel_size = 5;
        MorphologicalOperationParameters parameters = { &source_image, &kernel_size, &operation };
        namedWindow("Press C to close and S to save");
        createTrackbar("Kernel size", "Press C to close and S to save", &kernel_size, 10, onSizeMorphologicalOperationChange, &parameters);
        Mat edited_image = applyMorphologicalOperation(source_image, kernel_size, operation);
        imshow("Press C to close and S to save", edited_image);
        int k = waitKey(0);
        if (k == 99) {
            destroyAllWindows();
        } else if (k == 115) {
            string edited_image_filename = "/mnt/c/Users/alexa/OneDrive - ISEP/Documents/A2/Applications Multimédias/Projet/Rendu/Code/Saved Images/" + operation + source_image_filename;
            imwrite(edited_image_filename, edited_image);
            destroyAllWindows();
        } else {
            printErrorMessage();
        }
    }

}

struct BrightnessOperationParameters {
    Mat* source_image;
    int* factor;
    String* operation;
};

void onFactorChange(int value, void* userData) {
    auto* parameters = static_cast<BrightnessOperationParameters*>(userData);
    Mat* source_image = parameters->source_image;
    int* factor = parameters->factor;
    String operation = *(parameters->operation);
    *factor = value;
    if (*factor > 0) {
        imshow("Press C to close and S to save", applyBrightnessOperation(*source_image, *factor, operation));
    }
}

Mat applyBrightnessOperation(const Mat& source_image, int factor, const String& operation) {
    Mat edited_image;
    if (operation == "Lighted") {
        source_image.convertTo(edited_image, -1, 1,factor);
    } else if (operation == "Darked") {
        source_image.convertTo(edited_image, -1, 1,-factor);
    }
    return edited_image;
}

void brightnessOperation(const string& filename, String operation) {
    Mat source_image = imread(filename);
    filesystem::path path(filename);
    String source_image_filename = path.filename().string();
    int factor = 5;
    BrightnessOperationParameters parameters = { &source_image, &factor, &operation };
    namedWindow("Press C to close and S to save");
    createTrackbar("Factor", "Press C to close and S to save", &factor, 200, onFactorChange, &parameters);
    Mat edited_image = applyBrightnessOperation(source_image, factor, operation);
    imshow("Press C to close and S to save", edited_image);
    int k = waitKey(0);
    if (k == 99) {
        destroyAllWindows();
    } else if (k == 115) {
        string edited_image_filename = "/mnt/c/Users/alexa/OneDrive - ISEP/Documents/A2/Applications Multimédias/Projet/Rendu/Code/Saved Images/" + operation + source_image_filename;
        imwrite(edited_image_filename, edited_image);
        destroyAllWindows();
    } else {
        printErrorMessage();
    }
}

struct CannyEdgeDetectionParameters {
    Mat* source_image;
    int* threshold1;
};

void onSizeCannyEdgeDetectionChange(int value, void* userData) {
    auto* parameters = static_cast<CannyEdgeDetectionParameters*>(userData);
    Mat* source_image = parameters->source_image;
    int* threshold1 = parameters->threshold1;
    *threshold1 = value;
    if (*threshold1 > 0) {
        imshow("Press C to close and S to save", applyCannyEdgeDetection(*source_image, *threshold1));
    }
}

Mat applyCannyEdgeDetection(const Mat& source_image, int threshold1) {
    Mat edited_image;
    Canny(source_image, edited_image, threshold1, threshold1*2, 3);
    return edited_image;
}

void cannyEdgeDetection(const string& filename) {
    Mat source_image = imread(filename);
    filesystem::path path(filename);
    String source_image_filename = path.filename().string();
    int threshold1 = 100;
    MorphologicalOperationParameters parameters = { &source_image, &threshold1 };
    namedWindow("Press C to close and S to save");
    createTrackbar("Lowest threshold", "Press C to close and S to save", &threshold1, 1000, onSizeCannyEdgeDetectionChange, &parameters);
    Mat edited_image = applyCannyEdgeDetection(source_image, threshold1);
    imshow("Press C to close and S to save", edited_image);
    int k = waitKey(0);
    if (k == 99) {
        destroyAllWindows();
    } else if (k == 115) {
        string edited_image_filename = "/mnt/c/Users/alexa/OneDrive - ISEP/Documents/A2/Applications Multimédias/Projet/Rendu/Code/Saved Images/CannyEdgeDetected" + source_image_filename;
        imwrite(edited_image_filename, edited_image);
        destroyAllWindows();
    } else {
        printErrorMessage();
    }
}

void printInputTypeSelection(const string& filename) {
    Mat source_image = imread(filename);
    cout << endl;
    cout << "These are the original dimensions : " << source_image.rows << " x " << source_image.cols << endl;
    int input_type;
    cout << endl;
    cout << "Which type of input do you want to use ?" << endl;
    cout << "(1) Factor " << "\U0001F4A5" << " Chose this type if you want to keep the aspect ratio !" << endl;
    cout << "(2) Height & Width " << "\U0001F4A5" << " Chose this type if you want new specific dimensions !" << endl;
    cout << "[3] Exit" << endl;
    cout << endl;
    cout << " > ";
    cin >> input_type;
    switch (input_type) {
        case 1:
            printResizingOperationWithFactor(filename);
            break;
        case 2:
            printResizingOperationTwoDimensions(filename);
            break;
        default:
            printErrorMessage();
    }
}

void printResizingOperationWithFactor(const string& filename) {
    int functionality_index;
    cout << "What functionality do you want to use ?" << endl;
    cout << "(1) Increase" << endl;
    cout << "(2) Decrease" << endl;
    cout << "[3] Go back" << endl;
    cout << endl;
    cout << " > ";
    cin >> functionality_index;
    int factor;
    cout << endl;
    cout << "What is the factor (as a percentage) ?" << endl;
    cout << endl;
    cout << " > ";
    cin >> factor;
    switch (functionality_index) {
        case 1:
            resizingOperationWithFactor(filename, factor, "Increased");
            break;
        case 2:
            resizingOperationWithFactor(filename, factor, "Decreased");
            break;
        case 3:
            break;
        default:
            printErrorMessage();
    }
}


void resizingOperationWithFactor(const string& filename, double factor, const String& operation) {
    Mat source_image = imread(filename);
    filesystem::path path(filename);
    String source_image_filename = path.filename().string();
    namedWindow("Press C to close and S to save");
    Mat edited_image = applyResizingOperationWithFactor(source_image, factor, operation);
    imshow("Press C to close and S to save", edited_image);
    int k = waitKey(0);
    if (k == 99) {
        destroyAllWindows();
    } else if (k == 115) {
        string edited_image_filename = "/mnt/c/Users/alexa/OneDrive - ISEP/Documents/A2/Applications Multimédias/Projet/Rendu/Code/Saved Images/ResizedWithFactor" + source_image_filename;
        imwrite(edited_image_filename, edited_image);
        destroyAllWindows();
    } else {
        printErrorMessage();
    }
}

Mat applyResizingOperationWithFactor(const Mat& source_image, double factor, const String& operation) {
    Mat edited_image;
    double temp_factor;
    if (operation == "Increased") {
        temp_factor = (1 + (factor/100));
        resize(source_image, edited_image, Size(), temp_factor, temp_factor);
    } else if (operation == "Decreased") {
        temp_factor = (1 - (factor/100));
        resize(source_image, edited_image, Size(), temp_factor, temp_factor);
    }
    return edited_image;
}

void printResizingOperationTwoDimensions(const string& filename) {
    int height;
    cout << endl;
    cout << "What is the new height ?" << endl;
    cout << endl;
    cout << " > ";
    cin >> height;
    int width;
    cout << endl;
    cout << "What is the new width ?" << endl;
    cout << endl;
    cout << " > ";
    cin >> width;
    resizingOperationWithTwoDimensions(filename, height, width);
}

void resizingOperationWithTwoDimensions(const string& filename, int height, int width) {
    Mat source_image = imread(filename);
    filesystem::path path(filename);
    String source_image_filename = path.filename().string();
    namedWindow("Press C to close and S to save");
    Mat edited_image = applyResizingOperationWithTwoDimensions(source_image, height, width);
    imshow("Press C to close and S to save", edited_image);
    int k = waitKey(0);
    if (k == 99) {
        destroyAllWindows();
    } else if (k == 115) {
        string edited_image_filename = "/mnt/c/Users/alexa/OneDrive - ISEP/Documents/A2/Applications Multimédias/Projet/Rendu/Code/Saved Images/ResizedWithTwoDimensions" + source_image_filename;
        imwrite(edited_image_filename, edited_image);
        destroyAllWindows();
    } else {
        printErrorMessage();
    }
}

Mat applyResizingOperationWithTwoDimensions(const Mat& source_image, int height, int width) {
    Mat edited_image;
    resize(source_image, edited_image, Size(width, height));
    return edited_image;
}

void stitchingOperation(const string &directory) {
    vector<string> filenames;
    vector<Mat> source_images;
    glob(directory + "/*.jpg", filenames);
    for (const auto& filename : filenames) {
        Mat source_image = imread(filename);
        if (source_image.empty()) {
            cout << "Can't read image '" << "'\n";
        }
        source_images.push_back(source_image);
    }
    namedWindow("Press C to close and S to save");
    Mat edited_image;
    Ptr<Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA);
    Stitcher::Status status = stitcher->stitch(source_images, edited_image);
    if (status != Stitcher::OK) {
        cout << "Can't stitch images\n";
    }
    imshow("Press C to close and S to save", edited_image);
    int k = waitKey(0);
    if (k == 99) {
        destroyAllWindows();
    } else if (k == 115) {
        string edited_image_filename = "/mnt/c/Users/alexa/OneDrive - ISEP/Documents/A2/Applications Multimédias/Projet/Rendu/Code/Saved Images/Stitched.jpg";
        imwrite(edited_image_filename, edited_image);
        destroyAllWindows();
    } else {
        printErrorMessage();
    }

}
