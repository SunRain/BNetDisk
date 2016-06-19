
.pragma library

function categoryToIcon(category) {
    switch (category) {
    case 1:
        return "av/movie";
    case 3:
        return "image/image";
    case 4:
        return "image/texture";
    case 5:
        return "navigation/apps";
    case 7:
        return "file/attachment";
    case 8:
        return "image/music_note";
    default:
        return "file/attachment"
    }
}
