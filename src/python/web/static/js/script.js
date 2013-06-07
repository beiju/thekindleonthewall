function kindle(scope, updater) {
  scope.tabs = [
    {'heading': "Time & Weather", 'slug': "time-weather"},
    {'heading': "Calendar & Person", 'slug': "calendar-person"}
  ];
  scope.autoAdvance = {};
}
 
kindle.$inject = ['$scope' ,'updater'];

angular.module('Kindle', ['updater', 'ui.bootstrap']);