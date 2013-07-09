var thekindleonthewall = angular.module('thekindleonthewall', ['StateManager', 'be.poller', 'be.gadgets', 'skycons']);

thekindleonthewall.constant('config', {
	'autoTransitionDelay': 10000,
	'pollInterval': 3000
});

thekindleonthewall.controller('MainController', function ($scope, $state, $stateParams, Poller, config) {
    $scope.$state = $state;
    $scope.$stateParams = $stateParams;
    $scope.lastUpdate = 0;
    
    $scope.data = { 'initial' : true };
    
    Poller
    	.attach($scope.data)
    	.send(function() {
    		return { 'lastUpdate': $scope.lastUpdate };
    	})
    	.interceptor(function(data) {
    		if (data) data.initial = false;
    		
    		return data;
    	})
    	.afterTick(function (data) {
    		if (data.calendar && data.calendar.err_code && data.calendar.err_code == "not_authorized") {
    			// Redirect to authorization
    			window.location = '/login';
    		}
    		
    		$scope.lastUpdate = Math.round(Date.now() / 1000); // Servers usually track time in seconds, not milliseconds
    	})
    	.start(config.pollInterval);
});