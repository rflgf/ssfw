A humble discrete event simulation tool developed for a uni subject. SSFW stands for **Simple Simulation Framework**.

## FAQ

### A big portion of the model seems to have redundant data representation in the `Renderer` and `Simulation` modules. Why is that?
The goal for this application was mainly to serve as a framework rather than a tool in and of itself, so I tried to keep the simulation core functionalities about as decoupled from the GUI as possible. On top of that, I found it easier to reason about these aspects separately (though the graphics rendering code *is* quite questionable -- it wasn't the main focus of this project!).

### You can't just make a FAQ out of one question, specially one no one has ever actually asked.
Well but I just did didn't I. [Here, have a photo of this very endearing fellow.](https://media.springernature.com/w300/springer-static/image/art%3A10.1038%2Fnature.2013.13519/MediaObjects/41586_2013_Article_BFnature201313519_Figa_HTML.jpg)