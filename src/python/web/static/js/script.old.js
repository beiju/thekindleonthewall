$(function(){ //DOM Ready
 
    $(".gridster ul").gridster({
        widget_margins: [5, 5],
        widget_base_dimensions: [90, 64]
    });
    
    // Start the update cycle (this function will perpetuate itself)
    update();
 
});

var updateCount = 0;

function update() { // will be called by google API the first time, then by itself
	////// Time //////
	var now = new Date(),
		timeString = (now.getHours() % 12 == 0 ? '12' : now.getHours() % 12) + ':' + (now.getMinutes()<10 ? '0' : '') + now.getMinutes();
	$('#time.fill').html(timeString).removeClass('invisible');
	
	////// DAKA Hours //////
	if (updateCount % 60 == 0) { // every 60 updates -> every hour
		queryCalendar({
			calendar: '0cto0462lqrpt673m51bf1ucuk@group.calendar.google.com',
			eventOngoing: function(event) {
				$('#daka').removeClass('invisible');
				$('#daka-status').html('open for '+event.summary);
				$('#daka-change-text').html('close');
				$('#daka-change-time').html(moment(event.end.dateTime).fromNow(true));
			},
			nextEvent: function(event) {
				$('#daka').removeClass('invisible');
				$('#daka-status').html('closed');
				$('#daka-change-text').html('reopen');
				$('#daka-change-time').html(moment(event.start.dateTime).fromNow(true));
			},
			noMatch: function(event) {
				$('#daka').addClass('invisible');
				$('#daka-status').html('unknown');
				$('#daka-change-text').html('?');
				$('#daka-change-time').html('?');
			}
		});
		
		queryCalendar({
			calendar: 'ieqe1kvtb6narapqoafv59umog@group.calendar.google.com',
			eventOngoing: function(event) {
				$('#spoon').removeClass('invisible');
				$('#spoon-status').html('open for '+event.summary);
				$('#spoon-change-text').html('close');
				$('#spoon-change-time').html(moment(event.end.dateTime).fromNow(true));
			},
			nextEvent: function(event) {
				$('#spoon').removeClass('invisible');
				$('#spoon-status').html('closed');
				$('#spoon-change-text').html('reopen');
				$('#spoon-change-time').html(moment(event.start.dateTime).fromNow(true));
			},
			noMatch: function(event) {
				$('#spoon').addClass('invisible');
				$('#spoon-status').html('unknown');
				$('#spoon-change-text').html('?');
				$('#spoon-change-time').html('?');
			}
		});
		
	}
	
	////// Calendars //////
	if (updateCount % 5 == 0) { // every 5 updates -> every 5 minutes
		// Will
		getCalendarInfo('will', [
			'488or1ai5vadl5psti3iq8ipgs@group.calendar.google.com', // work
			'beiju.i.am@gmail.com', // personal
			't7ijq9al3asosqh1jnp93hvgdk@group.calendar.google.com' // class
		]);
		
		getCalendarInfo('ian', [
			'sodbfdhm4q7api4qvf5h5k7rlg@group.calendar.google.com', // social
			'36gqite9pam369c6mknuttgsqg@group.calendar.google.com', // work
			'achgl7e3m1pokdo8o1uqis70fk@group.calendar.google.com', // ACM
			'jnqo9lo8efm5ogj78pr176qstg@group.calendar.google.com', // WPI Extracurricular
			'a82i41iavlvd37d9fnrofklrms@group.calendar.google.com', // WPI Schoolwork
			'ianonavy@gmail.com'
		]);
	}
	
	////// Meal Plan Balances //////
	if (updateCount % 300 == 0) { // every 300 updates -> every 5 hours
		updateMealPlan('will', 63121, 'functionCall');
		updateMealPlan('ian', 63120, 'ian');
	}
	
	function updateMealPlan(person, port, callbackName) {
		var termStartDate = moment("2013-01-09"),
			termEndDate = moment("2013-05-31"),
			dayOfSemester = moment().diff(termStartDate, 'days'),
			semesterLength = termEndDate.diff(termStartDate,'days'),
			numOpts = {
				'readOnly': true, 
				'width': 80, 
				'height': 80, 
				'fgColor': '#ddd', 	
				'bgColor': '#888', 
				'inputColor': '#000'
			}, dateOpts = { 
				'displayInput': false, 
				'readOnly': true,
				'width': 80,
				'height': 80,
				'thickness': .05,
				'fgColor': '#ddd',
				'bgColor': '#222',
				'angleOffset': 360 * dayOfSemester / semesterLength,
				'angleArc': 360 * (1 - dayOfSemester / semesterLength)
			};
			
		$.ajax('http://cccwork3.wpi.edu:'+port+'/', {
			dataType: 'jsonp',
			jsonp: false,
			jsonpCallback: callbackName,
			success: function(response) { //! put the stuff in the val()
				$('.'+person+'.normal-meals .fill.number').val(87).knob(_.extend(numOpts, {'max': 190}));
				$('.'+person+'.special-meals .fill.number').val(18).knob(_.extend(numOpts, {'max': 32}));
				$('.'+person+'.bonus-points .fill.number').val(50.3).knob(_.extend(numOpts, {'max': 100}));
				   
				$('.'+person+' .fill.days').knob(dateOpts);
				   
				$('.'+person+'.meals').removeClass('invisible');
			}
		});
		$('.'+person+'.normal-meals .fill.number').val(87).knob(_.extend(numOpts, {'max': 190}));
		$('.'+person+'.special-meals .fill.number').val(18).knob(_.extend(numOpts, {'max': 32}));
		$('.'+person+'.bonus-points .fill.number').val(50.3).knob(_.extend(numOpts, {'max': 100}));
		   
		$('.'+person+' .fill.days').knob(dateOpts);
		   
		$('.'+person+'.meals').removeClass('invisible');
		
	}
	
	////// Update Yourself //////
	window.setTimeout(update, 61000 - Date.now() % 60000);
	console.log("last updated at", now.getSeconds(), "seconds after the minute, next update in", 61000 - Date.now() % 60000, "ms");
	
	updateCount++;
}

var calQueryQueue = [], calAuthenticating = false, authenticationAttempts = 0;
function queryCalendar(options) {
	if (options) calQueryQueue.push(options);
	
	if (typeof oauthToken != 'undefined' && oauthToken && oauthToken.access_token) {
		processCalQueue();
	} else if (!calAuthenticating) {
		if (typeof oauthToken == 'undefined' || !oauthToken) oauthToken = JSON.parse(localStorage.getItem('oauthToken'));
		
		if (oauthToken) {
			processCalQueue();
		} else {
			// try to get already set token
			gapi.auth.authorize({
				client_id: '1068849907100-or72tfuoovok2ircqstcvpc7bd4i18sh.apps.googleusercontent.com', 
				scope: 'https://www.googleapis.com/auth/calendar.readonly', 
				immediate: true
			}, function(result) {
				if (!result || !result.access_token) {
					gapi.auth.authorize({
						client_id: '1068849907100-or72tfuoovok2ircqstcvpc7bd4i18sh.apps.googleusercontent.com', 
						scope: 'https://www.googleapis.com/auth/calendar.readonly', 
						immediate: false
					}, function(result) {
						if (result && result.access_token) {
							oauthToken = result;
							gapi.auth.setToken(oauthToken);
							localStorage.setItem('oauthToken', JSON.stringify(oauthToken));
							processCalQueue();
						}
					});
				} else { // token found
					oauthToken = result;
					gapi.auth.setToken(oauthToken);
					localStorage.setItem('oauthToken', JSON.stringify(oauthToken));
					processCalQueue();
				}
			});
		}
	}
}

function processCalQueue() {
	while (calQueryQueue.length > 0) {
		var opts = calQueryQueue.shift();
		
		$.ajax({
			url: 'https://www.googleapis.com/calendar/v3/calendars/'+encodeURIComponent(opts.calendar)+'/events', 
			data: {
				orderBy: 'startTime',
				singleEvents: true,
				timeMax: moment().add('days', 4).format(),
				timeMin: moment().subtract('days', 4).format(),
				key: 'AIzaSyBEbwqiYKaGOzpdcU6PHPJZeL3ThJYBOgs',
				access_token: encodeURIComponent(oauthToken.access_token)
			},
			context: opts,
			success: function(data) {
				var opts = this;

				if (data.accessRole == 'freeBusyReader' && authenticationAttempts > 3) { // We accidentally performed an unauthenticated request
					delete oauthToken;
					localStorage.removeItem('oauthToken');
					calAuthenticating = false;
					queryCalendar(opts);
					authenticationAttempts++;
				} else if (data.items && data.items.length > 0) {
					for (var i = 0; i < data.items.length; i++) {
						var event = data.items[i],
							now = new Date(),
							startDate = new Date(event.start.dateTime),
							endDate = new Date(event.end.dateTime);
						
						if (startDate < now && endDate > now) {
							// Then the event is ongoing
							if (opts.eventOngoing) opts.eventOngoing(event, opts);
							break; // don't continue looking for the current event, we found it
						} else if (startDate > now && endDate > now) {
							// Then the event is in future (doo WEEE ooooo, DOOOooooOOOOO...)
							if (opts.nextEvent) opts.nextEvent(event, opts);
							break; // if we're at the future already, there's no current event, so stop looking
						}
					}
				} else {
					// No match is found for the date query
					if (opts.noMatch) opts.noMatch(opts);
				}
			}, error: function() { 
				opts.requestFailure(opts) 
			}
		});	
		
	}
}
var eventFound = [];
function getCalendarInfo(person, calendars) {
	for (var i = 0; i < calendars.length; i++) {
		eventFound[person] = false;
		queryCalendar({
			calendar: calendars[i],
			person: person, 
			eventOngoing: function(event, opts) {
				$('#'+opts.person+' .place').html(event.location);
				$('#'+opts.person+' .activity').html(event.summary);
				$('#'+opts.person+' .location').removeClass('invisible faded');
				eventFound[opts.person] = true;
				return;
			}, noMatch: function(opts) {
				if (!eventFound[opts.person]) {
					$('#'+opts.person+' .place').html('Location Unknown');
					$('#'+opts.person+' .activity').html('Unknown');
					$('#'+opts.person+' .location').removeClass('invisible').addClass('faded');
				}
			}, nextEvent: function(event, opts) {
				if (!eventFound[opts.person]) {
					$('#'+opts.person+' .place').html('Location Unknown');
					$('#'+opts.person+' .activity').html('Unknown');
					$('#'+opts.person+' .location').removeClass('invisible').addClass('faded');
				}
			},
			requestFailure: function(opts) {
				if (!eventFound[opts.person]) {
					$('#'+opts.person+' .location').addClass('invisible');
				}
			}
		});
	}
}