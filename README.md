# WeatherGen v1.0


The next project I wanted to give a bit of graphic's a go. I decided on the learning the SDL2 API, whilst building a C lib for it. During development I found the use of Arena Style memory
management to be fantasic and to show it off I built a little weather generator. Easily protatble to your projects!! Never has boring weather again :)

I built this little demo to show off the WeatherGen, download the Project, run the make file, and give it a go!

### Weather Effects

*Excuse the janky gifs, I promise the game play was smooth*

#### Rain

Spwans rain particals inside a Weather Box that fall to the ground with increasing speed as the drop size increases in relation to drops per second. Once in contact with collision, play out a little splash death animation

Adjustment controls
- Drop count per second
- Wind

![rain1](images/rain1.gif)

![rain2](images/rain2.gif)

#### Lightning

Spwans evolving Lightning Strikes within a Weather Box, evolves with relation intensity and strand count settings. Lights up any Rain particals in the scene when strike occurs

Adjustment controls
- Serverity (increaing the chance of strike and strand intensity)
- Strand Count (caps the maximum number of strands to spwan per strike)
- Delay (adds timed cooldown between strike coming back online)

![lightning1](images/lightning1.gif)

![lightning2](images/lightning2.gif)


#### Snow 

Spwans snow particals inside a Weather Box that fall down with increasing speed and size in relation to the per second spwan count. Once in contact with collision they build up a snow pile, as long as there is room in the laying snow count (1/4 of max spwan rate). 
When another collision object comes into collision with teh snow pile they had a death animation taking into account which direction they are approached from.

Adjustment controls
- Snow count per second
- Wind

![snow2](images/snow1.gif)

![snow](images/snow3.gif)

![snow1](images/snow2.gif)


### Installation


