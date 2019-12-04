# Neural-Network-signature-classification
# Android/Java application with C++ neural network for handwritten signature classification.

## Hi, this was the final project of my graduation.
## I hope you find something to help you.

In the Documentation paste, you will find my article (Artigo.docx, written in Portuguese/Brazil).

The project have the objective of training a neural network to identify if the user is registered to realize login.

There actualy two project: one is a C++ project made in Visual Studio and another is a android/java project made in Android Studio, where the neural network is a cpp (C++) class who run in a NDK and is being called via JNI.

The C++ project was made for runing in a computer to training the neural network for performance reasons.
To run it. You have to get your data in a txt in the next format:
![Format](/Prints-Readme/DataSetFormat.png?raw=true "Format")

The files are stored in the "\Local\CppProject\CppProject\DataSet" directory.
Where "in: 0.xxx 0.xxxx" is the  inputs and the "out: 0.0" are the desired outputs, the dataset does not need normalization because we normalize it later.

The interface of the C++ is simple. In the menu, you can choose the options of training and testing.
![MenuP](/Prints-Readme/Menu.png?raw=true "MenuP")

Chosing the training, you will have the choice to normalize the dataset and you can define the topology of your neural network.
![TopologyDefinition](/Prints-Readme/TopologyDefinition.png?raw=true "TopologyDefinition")

The condition to stop the training is set directly in the code. The one i was using was: mean squared error minor than 0.0005 and 100% accuracy in the desired outputs.
*//myNet.getRecentAverageError() >  0.0005 || countAccuracy < countDataSet*
After the condition is satisfied, the test is started.
![Testing](/Prints-Readme/Test.png?raw=true "Testing")

After the training. You will have in the same directory the files minMaxFile.txt that store the minimun and maximun values of the inputs for the normalization in the android device and the most important, bestWeight.txt, that store the weights who have the best results from your training with the topology on top.

![BestWeights](/Prints-Readme/bestWeights.png?raw=true "BestWeights")

The dataset is put in the device manually.
The android will read the file in a separete directory created by the app "\UserSignature".
Store the bestWeight.txt and minMaxFile.txt in this directory to use the app.

The app is responsible for collection the data and do the actual classification of the signature.
![MenuAndroid](/Prints-Readme/Menu-Android.png?raw=true "MenuAndroid")

The first button is to do the feedforward and verify your signature/login. The C++ neural network run in a NDK and is being called via JNI.
The second is to collect a data of a new user and send the text document and the signatures pictures to an email.
The third is to data collection of non-users for a non-users dataset.

The forms to collect signatures are basically the same:
![Signature](/Prints-Readme/signature.png?raw=true "Signature")


**Information about the NN** 
The inputs are: total time of execution, finger lift amount, average signature execution time during finger pression periods, trajectory sampling (sum of the initial and final X Y coordinates), horizontal and vertical maximums and signature density.

*Signature density feature collect example*
![Signature density](/Prints-Readme/density.png?raw=true "Signature density")

Activation function: sigmoid.

Accuracy: 80% in test dataset and 90% in field test.


* Possible implementations:
  * Storage management change:
    * Changing text documents to store values for a database and automatic download of the weights and min/max values in the android device.
  * Inputs improvement:
    * Improvement in the signature feature capture (image processing and etc.).

