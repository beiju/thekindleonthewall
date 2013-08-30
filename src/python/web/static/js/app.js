var thekindleonthewall = angular.module('thekindleonthewall', ['StateManager', 'be.poller', 'be.timekeeper', 'be.gadgets', 'be.gauge', 'skycons']);

thekindleonthewall.constant('config', {
	'autoTransitionDelay': 10000,
	'pollInterval': 3000,
	'isKindle': /Linux armv7l/i.test(navigator.userAgent),
	'animationUpdateInterval': /Linux armv7l/i.test(navigator.userAgent) ? 1000 : 10
});

thekindleonthewall.controller('MainController', function ($scope, $state, $stateParams, Poller, Timekeeper, config) {
    $scope.$state = $state;
    $scope.$stateParams = $stateParams;
    $scope.config = config;
    $scope.lastUpdate = 0;
    
    // DATA
    $scope.data = { 'initial' : true };
    Poller
    	.attach($scope.data)
    	.send(function() {
    		return { 'lastUpdate': $scope.lastUpdate };
    	})
    	.interceptor(function(data) {
    		if (data) data.initial = false;
    		
    		if (data.calendar) {
    			angular.forEach(data.calendar, function(cal_obj, cal_key) {
    				angular.forEach(data.calendar[cal_key], function(evt_obj, evt_key) {
    					if (data.calendar[cal_key][evt_key].start_time) {
    						data.calendar[cal_key][evt_key].start_time = new Date(data.calendar[cal_key][evt_key].start_time);
    					}
    					if (data.calendar[cal_key][evt_key].end_time) {
    						data.calendar[cal_key][evt_key].end_time = new Date(data.calendar[cal_key][evt_key].end_time);
    					}
    				});
    			});
    		}
    		
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
    
    // TIMEKEEPING
	$scope.mainTimer = {
		timezone: 'PST',
		label: 'CA',
		time: new Date()
	}
	$scope.offsetTimer = {
		timezone: 'EST',
		label: 'MA',
		time: null
	}
    Timekeeper
    	.attach($scope.mainTimer, 'local')
    	.attach($scope.offsetTimer, 3);
});