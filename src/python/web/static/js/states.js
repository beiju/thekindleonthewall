angular.module('StateManager', ['ui.state'])
	.controller('TransitionController', function($scope, $state, $timeout, config) {
		// Automatically transition to the next state
		$scope.$on('$stateChangeSuccess', function(event, toState, toParams, fromState, fromParams){
			$timeout(function() {
				// If we're still in auto mode, complete the transition
				if ($state.$current.includes['auto']) $state.transitionTo('auto.'+toState.data.nextView);
			}, config.autoTransitionDelay);
		});
		
	})
	.config(['$urlRouterProvider', '$stateProvider', function($urlRouterProvider, $stateProvider){
		$stateProvider
		    .state('auto', {
		        url: "/auto", // root route
		        abstract: true,
		        views: {
		            "main": { templateUrl: "auto.html", controller: 'TransitionController' }
		        }
		    })
		    .state('personal-info', {
		        url: "/personal-info",
		        views: {
		            "main": { templateUrl: "personal-info.html" }
		        }
		    })
		    .state('general-info', {
		        url: "/general-info",
		        views: {
		            "main": { templateUrl: "general-info.html" }
		        },
		    })
		    .state('auto.personal-info', {
		        url: "/personal-info",
		        views: {
		            "main": { templateUrl: "personal-info.html" }
		        },
		        data: {
		        	nextView: 'general-info'
		        }
		    })
		    .state('auto.general-info', {
		        url: "/general-info",
		        views: {
		            "main": { templateUrl: "general-info.html" }
		        },
		        data: {
		        	nextView: 'personal-info'
		        }
		        
		    });
	    $urlRouterProvider.otherwise('/auto/personal-info');
	}]);