#include<bits/stdc++.h>
using namespace std;

class Song{
private:
    string title;
    string artist;
    string path;
public:
    Song(const string &title, const string &artist, const string &path) : 
        title(title), artist(artist), path(path) {}

    string getSongTitle() const {
        return title;
    }

    string getSongArtist() const {
        return artist;
    }

    string getSongPath() const {
        return path;
    }
};

class Playlist{
private:
    string playlistName;
    vector<Song*> songs;
public:
    Playlist(const string& playName) : playlistName(playName) {}

    void addSongToPlaylist(Song *song) {
        if(find(songs.begin(), songs.end(), song) == songs.end()){
            songs.push_back(song);
        }
    }

    const vector<Song*>& getPlaylistSongs() const {
        return songs;
    }

    string getPlaylistName() const {
        return playlistName;
    }

    int getSize() const{
        return songs.size();
    }
};

class PlaylistManager{
private:
    unordered_map<string, Playlist*> playlists;
    PlaylistManager() {}
public:
    static PlaylistManager& getInstance(){
        static PlaylistManager instance;
        return instance;
    }

    Playlist* getPlaylist(const string &playlistName){
        if(playlists.count(playlistName)){
            return playlists[playlistName];
        }
        return nullptr;
    }

    void createPlaylist(const string &playlistName) {
        if(!playlists.count(playlistName)){
            Playlist* newPlay = new Playlist(playlistName);
            playlists[playlistName] = newPlay;
        }
    }

    void addSongToPlaylist(const string &playlistname, Song* s){
        if(playlists.count(playlistname)){
            auto &play = playlists[playlistname];
            play->addSongToPlaylist(s);
        }
    }

    ~PlaylistManager() {
        for(auto &it: playlists){
            delete it.second;
        }
        playlists.clear();
    }
};

class IAudioOutputDevice{
public:
    virtual void playAudio(Song *song) = 0;
    virtual ~IAudioOutputDevice() = default;
};

class Bluetooth{
public:
    void play(Song *song) {
        cout<<" Song is played via bluetooth : " << song->getSongTitle() <<endl;
    }
};

class BluetoothAdapter : public IAudioOutputDevice{
private:
    Bluetooth *bt;
public:
    void playAudio(Song *song) override {
        bt->play(song);
    }
};

class WiredSpeaker{
public:
    void play(Song *song) {
        cout<<" Song is played via WiredSpeaker : " << song->getSongTitle() <<endl;
    }
};

class WiredSpeakerAdapter : public IAudioOutputDevice{
private:
    WiredSpeaker *bt;
public:
    void playAudio(Song *song) override {
        bt->play(song);
    }
};

class Headphone{
public:
    void play(Song *song) {
        cout<<" Song is played via Headphone : " << song->getSongTitle() <<endl;
    }
};

class HeadphoneAdapter : public IAudioOutputDevice{
private:
    Headphone *bt;
public:
    void playAudio(Song *song) override {
        bt->play(song);
    }
};

enum class DeviceType { 
    BLUETOOTH, 
    WIRED,
    HEADPHONES 
};

class DeviceFactory{
public:
    static IAudioOutputDevice* createDevice(DeviceType dt) {
        if(dt == DeviceType::BLUETOOTH){
            return new BluetoothAdapter();
        }
        if(dt == DeviceType::WIRED){
            return new WiredSpeakerAdapter();
        }
        if(dt == DeviceType::HEADPHONES){
            return new HeadphoneAdapter();
        }

        return nullptr;
    }
};

class DeviceManager{
private:
    IAudioOutputDevice* currentOutputDevice;
    DeviceManager() {
        currentOutputDevice = nullptr;
    }
public:
    static DeviceManager& getInstance(){
        static DeviceManager instance;
        return instance;
    }

    void connect(DeviceType dt) {
        if(currentOutputDevice){
            delete currentOutputDevice;
        }

        currentOutputDevice = DeviceFactory::createDevice(dt);

        switch(dt) {
            case DeviceType::BLUETOOTH:
                cout<< "Bluetooth device connected" <<endl;
                break;
            case DeviceType::WIRED:
                cout<< "Wired device connected" <<endl;
                break;
            case DeviceType::HEADPHONES:
                cout<< "Headphones connected" <<endl;        
        }
    }

    IAudioOutputDevice* getDevice() const {
        if(!currentOutputDevice){
            throw runtime_error("No Device is connected currently");
        }
        return currentOutputDevice;
    }

    bool hasOutputDevice() {
        return currentOutputDevice != nullptr;
    }

    ~DeviceManager() {
        delete currentOutputDevice;
        currentOutputDevice = nullptr;
    }
};

class AudioEngine {
private:
    Song* currSong;
public:
    AudioEngine() {
        currSong = nullptr;
    }

    void playSong(IAudioOutputDevice* device, Song* song){
        currSong = song;
        device->playAudio(song);
    }

    Song* getCurrentSong() const {
        return currSong;
    }
};

class PlayStrategy{
public:
    virtual ~PlayStrategy() = default;
    virtual void setPlaylist(Playlist* pl) = 0;
    virtual bool hasNext() = 0;
    virtual Song* next() = 0;
    virtual bool hasPrev() = 0;
    virtual Song* prev() = 0;
    virtual void addToNext(Song* song) {}
};

class SequentialStrategy : public PlayStrategy{
private:
    Playlist* currList;
    int currIndex;
public:
    SequentialStrategy() {
        currList = nullptr;
        currIndex = -1;
    }

    void setPlaylist(Playlist* pl) override{
        currList = pl;
        currIndex = -1;
    }

    bool hasNext() override{
        return currList && ((currIndex + 1) < currList->getSize());
    }

    Song* next() override {
        if(!currList or currList->getSize() == 0) {
            throw runtime_error("No Playlist loaded or playlist is empty");
        }

        currIndex ++;
        return currList->getPlaylistSongs()[currIndex];
    }

    bool hasPrev() override{
        return ((currIndex - 1) >= 0);
    }

    Song* prev() override {
        if(!currList or currList->getSize() == 0) {
            throw runtime_error("No Playlist loaded or playlist is empty");
        }

        currIndex --;
        return currList->getPlaylistSongs()[currIndex];
    }
};

class RandomStrategy : public PlayStrategy{
private:
    Playlist *currList;
    vector<Song*> remainSongs;
    stack<Song*> history;
public:
    RandomStrategy() {
        currList = nullptr;
        srand((unsigned)time(nullptr));
    }

    void setPlaylist(Playlist* pl) override {
        currList = pl;
        if(!currList or pl->getSize() == 0) return ;

        remainSongs = pl->getPlaylistSongs();
        history = stack<Song*> ();
    }

    bool hasNext() override {
        return currList and !remainSongs.empty();
    }

    Song* next() {
        if(!currList or currList->getSize() == 0){
            throw runtime_error("No playlist loaded or playlist is empty.");            
        }
        if(remainSongs.empty()){
            throw runtime_error("No songs left to play");            
        }

        int nextIndex = rand() % remainSongs.size();
        Song* nextSong = remainSongs[nextIndex];

        swap(remainSongs[nextIndex], remainSongs.back());
        remainSongs.pop_back();

        history.push(nextSong);
        return nextSong;
    }

    bool hasPrev() override {
        return history.size() >= 0;
    }

    Song* prev() override {
        if (history.empty()) {
            throw runtime_error("No previous song available.");
        }

        Song* song = history.top();
        history.pop();
        return song;
    }
};

class CustomQueueStrategy : public PlayStrategy {
private:
    Playlist* currList;
    int currIndex;
    queue<Song*> nextQueue;
    stack<Song*> history;

    Song* nextSequential() {
        if (currList->getSize() == 0) {
            throw runtime_error("Playlist is empty.");
        }
        currIndex = currIndex + 1;
        return currList->getPlaylistSongs()[currIndex];
    }

    Song* previousSequential() {
        if (currList->getSize() == 0) {
            throw runtime_error("Playlist is empty.");
        }
        currIndex = currIndex - 1;
        return currList->getPlaylistSongs()[currIndex];
    }

public:
    CustomQueueStrategy() {
        currList = nullptr;
        currIndex = -1;
    }

    void setPlaylist(Playlist* pl) override {
        currList = pl;
        currIndex = -1;

        nextQueue = queue<Song*> ();
        history = stack<Song*> ();
    }

    bool hasNext() override {
        return !nextQueue.empty() or (currList && (currIndex + 1) < currList->getSize());
    }

    Song* next() override {
        if (!currList or currList->getSize() == 0) {
            throw runtime_error("No playlist loaded or playlist is empty.");
        }   
        
        if (!nextQueue.empty()) {
            Song* s = nextQueue.front();
            nextQueue.pop();
            history.push(s);
            auto& list = currList->getPlaylistSongs();
            for (int i = 0; i < (int)list.size(); ++i) {
                if (list[i] == s) {
                    currIndex = i;
                    break;
                }
            }
            return s;
        }  
        
        return nextSequential();
    }

    bool hasPrev() override {
        if (!history.empty()) return true;
        return currList and ((currIndex - 1) >= 0);
    }
    
    Song* prev() override {
        if (!currList or currList->getSize() == 0) {
            throw runtime_error("No playlist loaded or playlist is empty.");
        }

        if (!history.empty()) {
            Song* s = history.top();
            history.pop();

            auto& list = currList->getPlaylistSongs();
            for (int i = 0; i < (int)list.size(); ++i) {
                if (list[i] == s) {
                    currIndex = i;
                    break;
                }
            }
            return s;
        }

        return previousSequential();
    }

    void addToNext(Song* song) override {
        if (!song) {
            throw runtime_error("Cannot enqueue null song.");
        }
        nextQueue.push(song);
    }
};

enum class PlayStrategyType { 
    SEQUENTIAL, 
    RANDOM, 
    CUSTOM_QUEUE 
};

class StrategyManager {
private:
    SequentialStrategy* sequentialStrategy;
    RandomStrategy* randomStrategy;
    CustomQueueStrategy* customQueueStrategy;

    StrategyManager() {
        sequentialStrategy = new SequentialStrategy();
        randomStrategy = new RandomStrategy();
        customQueueStrategy = new CustomQueueStrategy();
    }
public:
    static StrategyManager& getInstance() {
        static StrategyManager instance;
        return instance;
    }

    PlayStrategy* getStrategy(PlayStrategyType type) {
        if (type == PlayStrategyType::SEQUENTIAL) {
            return sequentialStrategy;
        } else if (type == PlayStrategyType::RANDOM) {
            return randomStrategy;
        } else {
            return customQueueStrategy;
        }
    }

    ~StrategyManager() {
        delete sequentialStrategy;
        delete randomStrategy;
        delete customQueueStrategy;
    }
};

class MusicPlayerFacade {
private:
    AudioEngine *aud;
    Playlist* loadedPlaylist;
    PlayStrategy* playStrat;

    MusicPlayerFacade() {
        loadedPlaylist = nullptr;
        playStrat = nullptr;
        aud = new AudioEngine();
    }
public:
    static MusicPlayerFacade& getInstance() {
        static MusicPlayerFacade instance;
        return instance;
    }
    ~MusicPlayerFacade() {
        delete aud;
    }

    void connectDevice(DeviceType dt){
        DeviceManager::getInstance().connect(dt);
    }

    void setPlayStrategy(PlayStrategyType strat) {
        playStrat = StrategyManager::getInstance().getStrategy(strat);
    }

    void loadPlaylist(const string& name) {
        loadedPlaylist = PlaylistManager::getInstance().getPlaylist(name);
        if (!playStrat) {
            throw runtime_error("Play strategy not set before loading.");
        }

        playStrat->setPlaylist(loadedPlaylist);
    }

    void playSong(Song* song){
        if(!DeviceManager::getInstance().hasOutputDevice()){
            throw runtime_error("No audio device connected.");            
        }

        IAudioOutputDevice* dev = DeviceManager::getInstance().getDevice();
        aud->playSong(dev, song);
    }

    void playAllTracks(){
        if(!loadedPlaylist){
            throw runtime_error("No playlist loaded.");
        }

        while(playStrat->hasNext()){
            Song* nextSong = playStrat->next();
            IAudioOutputDevice* dev = DeviceManager::getInstance().getDevice();
            aud->playSong(dev, nextSong);
        }

        cout << "Completed playlist: " << loadedPlaylist->getPlaylistName() << endl;
    }

    void playNextTrack() {
        if (!loadedPlaylist) {
            throw runtime_error("No playlist loaded.");
        }
        if(playStrat->hasNext()) {
            Song* nextSong = playStrat->next();
            IAudioOutputDevice* device = DeviceManager::getInstance().getDevice();
            aud->playSong(device, nextSong);
        }
        else {
            cout << "Completed playlist: " << loadedPlaylist->getPlaylistName() << endl;
        }
    }

    void playPreviousTrack() {
        if (!loadedPlaylist) {
            throw runtime_error("No playlist loaded.");
        }
        if(playStrat->hasPrev()) {
            Song* prevSong = playStrat->prev();
            IAudioOutputDevice* device = DeviceManager::getInstance().getDevice();
            aud->playSong(device, prevSong);
        }
        else {
            cout << "Completed playlist: " << loadedPlaylist->getPlaylistName() << endl;
        }
    }

    void enqueueNext(Song* song) {
        playStrat->addToNext(song);
    }
};

class MusicPlayerApplication {
private:
    vector<Song*> songLibrary;
    MusicPlayerApplication() {}

public:
    static MusicPlayerApplication& getInstance() {
        static MusicPlayerApplication instance;
        return instance;
    }

    ~MusicPlayerApplication() {
        for (Song* s : songLibrary) {
            delete s;
        }
        songLibrary.clear();
    }

    void createSongInLibrary(const string& title, const string& artist, const string& path) {
        Song* newSong = new Song(title, artist, path);
        songLibrary.push_back(newSong);
    }

    Song* findSongByTitle(const string& title) {
        for (Song* s : songLibrary) {
            if (s->getSongTitle() == title) {
                return s;
            }
        }
        return nullptr;
    }

    void createPlaylist(const string& playlistName) {
        PlaylistManager::getInstance().createPlaylist(playlistName);
    }

    void addSongToPlaylist(const string& playlistName, const string& songTitle) {
        Song* song = findSongByTitle(songTitle);
        if (!song) {
            throw runtime_error("Song \"" + songTitle + "\" not found in library.");
        }
        PlaylistManager::getInstance().addSongToPlaylist(playlistName, song);
    }    

    void connectAudioDevice(DeviceType deviceType) {
        MusicPlayerFacade::getInstance().connectDevice(deviceType);
    }

    void selectPlayStrategy(PlayStrategyType strategyType) {
        MusicPlayerFacade::getInstance().setPlayStrategy(strategyType);
    }

    void loadPlaylist(const string& playlistName) {
        MusicPlayerFacade::getInstance().loadPlaylist(playlistName);
    }

    void playSingleSong(const string& songTitle) {
        Song* song = findSongByTitle(songTitle);
        if (!song) {
            throw runtime_error("Song \"" + songTitle + "\" not found.");
        }
        MusicPlayerFacade::getInstance().playSong(song);
    }

    void playAllTracksInPlaylist() {
        MusicPlayerFacade::getInstance().playAllTracks();
    }

    void playPreviousTrackInPlaylist() {
        MusicPlayerFacade::getInstance().playPreviousTrack();
    }

    void queueSongNext(const string& songTitle) {
        Song* song = findSongByTitle(songTitle);
        if (!song) {
            throw runtime_error("Song \"" + songTitle + "\" not found.");
        }
        MusicPlayerFacade::getInstance().enqueueNext(song);
    }
};

int main() {
    try {
        auto application = MusicPlayerApplication::getInstance();

        // Populate library
        application.createSongInLibrary("Kesariya",  "Arijit Singh",  "/music/kesariya.mp3");
        application.createSongInLibrary("Chaiyya Chaiyya",   "Sukhwinder Singh",  "/music/chaiyya_chaiyya.mp3");
        application.createSongInLibrary("Tum Hi Ho",  "Arijit Singh",  "/music/tum_hi_ho.mp3");
        application.createSongInLibrary("Jai Ho",  "A. R. Rahman",  "/music/jai_ho.mp3");
        application.createSongInLibrary("Zinda",  "Siddharth Mahadevan",  "/music/zinda.mp3");

        // Create playlist and add songs
        application.createPlaylist("Bollywood Vibes");
        application.addSongToPlaylist("Bollywood Vibes", "Kesariya");
        application.addSongToPlaylist("Bollywood Vibes", "Chaiyya Chaiyya");
        application.addSongToPlaylist("Bollywood Vibes", "Tum Hi Ho");
        application.addSongToPlaylist("Bollywood Vibes", "Jai Ho");

        // Connect device
        application.connectAudioDevice(DeviceType::BLUETOOTH);

        //Play/pause a single song
        application.playSingleSong("Zinda");

        cout << "\n-- Sequential Playback --\n";
        application.selectPlayStrategy(PlayStrategyType::SEQUENTIAL);
        application.loadPlaylist("Bollywood Vibes");
        application.playAllTracksInPlaylist();

        cout << "\n-- Random Playback --\n";
        application.selectPlayStrategy(PlayStrategyType::RANDOM);
        application.loadPlaylist("Bollywood Vibes");
        application.playAllTracksInPlaylist();

        cout << "\n-- Custom Queue Playback --\n";
        application.selectPlayStrategy(PlayStrategyType::CUSTOM_QUEUE);
        application.loadPlaylist("Bollywood Vibes");
        application.queueSongNext("Kesariya");
        application.queueSongNext("Tum Hi Ho");
        application.playAllTracksInPlaylist();

        cout << "\n-- Play Previous in Sequential --\n";
        application.selectPlayStrategy(PlayStrategyType::SEQUENTIAL);
        application.loadPlaylist("Bollywood Vibes");
        application.playAllTracksInPlaylist();

        application.playPreviousTrackInPlaylist();
        application.playPreviousTrackInPlaylist();

    } catch (const exception& error) {
        cerr << "Error: " << error.what() << endl;
    }
    return 0;
}