/* StoreAdapter is a utility to initial Store singleton objects and setup dependence
 */
import QtQuick 2.0
import QuickFlux 1.0
import "../StoreWorkers"
//import "../storeworkers"

AppListener {

    Item {
        id: container

        FileOperationStoreWorker{}
        ShareStoreWorker{}
    }

    onDispatched: {
        var workers = container.data;

        for (var i in workers) {
            workers[i].dispatched(type, message);
        }
    }
}
